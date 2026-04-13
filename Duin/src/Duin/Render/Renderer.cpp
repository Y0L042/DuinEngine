#include "dnpch.h"
#include "Renderer.h"

#include "Camera.h"

#include <external/imgui.h>

#include "Duin/Core/Debug/DNLog.h"
#include "Duin/Core/Application.h"
#include "Duin/Core/Maths/MathsModule.h"

namespace duin
{

// ---------------------------------------------------------------------------
// Internal types
// ---------------------------------------------------------------------------

struct GeometryBufferHandle
{
    UUID uuid;
    RHIVertexBufferHandle vbh;
    RHIIndexBufferHandle ibh;

    GeometryBufferHandle() = default;
    GeometryBufferHandle(RHIVertexBufferHandle vbh_, RHIIndexBufferHandle ibh_) : vbh(vbh_), ibh(ibh_)
    {
    }
};

// ---------------------------------------------------------------------------
// Static state
// ---------------------------------------------------------------------------

static bool RENDER_CONTEXT_AVAILABLE = false;
static UUID DEFAULT_SHADERPROGRAM_UUID;
static std::vector<GeometryBufferHandle> geometryBufferList;
static std::unordered_map<UUID, ShaderProgram> shaderProgramMap;
static RHIEncoder *encoder = nullptr;
static RenderState globalRenderState;
static std::vector<RenderState> globalRenderStateStack;

static void CreateGeometryBuffers();
static GeometryBufferHandle GetGeometryBufferHandle(RenderGeometryType::Type type);

// Row-major Matrix to column-major float[16] for RHI.
static void MatrixToFloat16(Matrix m, float *r)
{
    r[0] = m.m0;
    r[1] = m.m4;
    r[2] = m.m8;
    r[3] = m.m12;

    r[4] = m.m1;
    r[5] = m.m5;
    r[6] = m.m9;
    r[7] = m.m13;

    r[8] = m.m2;
    r[9] = m.m6;
    r[10] = m.m10;
    r[11] = m.m14;

    r[12] = m.m3;
    r[13] = m.m7;
    r[14] = m.m11;
    r[15] = m.m15;
}

// ---------------------------------------------------------------------------
// RenderTexture implementation
// ---------------------------------------------------------------------------

RenderTexture::RenderTexture(int width_, int height_, UUID textureUUID_)
    : width(static_cast<uint16_t>(width_)), height(static_cast<uint16_t>(height_)), textureUUID(textureUUID_)
{
    texture = RHICreateTexture2D(width, height);
    frameBuffer = RHICreateFrameBuffer(texture);
}

bool RenderTexture::IsValid()
{
    return texture.IsValid() && frameBuffer.IsValid() && (width > 0) && (height > 0);
}

void RenderTexture::Destroy()
{
    if (!IsRenderContextAvailable())
    {
        return;
    }

    textureUUID = UUID::INVALID;
    width = 0;
    height = 0;

    RHIDestroyTexture(texture);
    texture = RHITextureHandle{};

    RHIDestroyFrameBuffer(frameBuffer);
    frameBuffer = RHIFrameBufferHandle{};
}

// ---------------------------------------------------------------------------
// Lifecycle
// ---------------------------------------------------------------------------

void InitRenderer()
{
    RHIInit();

    // Load default shaders
    RHIShaderHandle vsh =
        RHILoadShader("C:/Projects/CPP_Projects/Duin/Duin/src/Duin/Resources/shaders/dx11/vs_cubes.bin");
    RHIShaderHandle fsh =
        RHILoadShader("C:/Projects/CPP_Projects/Duin/Duin/src/Duin/Resources/shaders/dx11/fs_cubes.bin");

    // Create default shader program
    RHIProgramHandle program = RHICreateProgram(vsh, fsh, true);
    ShaderProgram shaderProgram(vsh, fsh, program);
    shaderProgramMap[shaderProgram.uuid] = shaderProgram;
    DEFAULT_SHADERPROGRAM_UUID = shaderProgram.uuid;

    CreateGeometryBuffers();

    DN_CORE_INFO("Renderer initialised.");
}

void CleanRenderer()
{
    // TODO
    // Clean buffers
}

void ResetRenderer()
{
    RENDER_CONTEXT_AVAILABLE = false;

    for (auto &buf : geometryBufferList)
    {
        RHIDestroyVertexBuffer(buf.vbh);
        RHIDestroyIndexBuffer(buf.ibh);
    }
    geometryBufferList.clear();

    for (auto &[uuid, prog] : shaderProgramMap)
    {
        RHIDestroyProgram(prog.program);
    }
    shaderProgramMap.clear();
    DEFAULT_SHADERPROGRAM_UUID = UUID{0};

    if (encoder)
    {
        RHIEndEncoder(encoder);
        encoder = nullptr;
    }

    RHIShutdown();

    globalRenderState = RenderState{};
    globalRenderStateStack.clear();
}

bool IsRenderContextAvailable()
{
    return RENDER_CONTEXT_AVAILABLE;
}

void SetRenderContextAvailable(bool available)
{
    RENDER_CONTEXT_AVAILABLE = available;
}

// ---------------------------------------------------------------------------
// Render modes
// ---------------------------------------------------------------------------

void BeginDraw3D(Camera &camera)
{
    globalRenderStateStack.push_back(globalRenderState);
    const RHIViewId viewID = RHI_VIEW_3D;

    // Get camera matrices
    Matrix viewMtx = GetCameraViewMatrix(camera.GetImpl());
    Matrix projMtx = GetCameraProjectionMatrix(camera.GetImpl());

    // Set view transform
    float view[16];
    float proj[16];
    MatrixToFloat16(viewMtx, view);
    MatrixToFloat16(projMtx, proj);
    RHISetViewTransform(viewID, view, proj);
    RHISetViewRect(viewID, 0, 0, (uint16_t)GetWindowWidth(), (uint16_t)GetWindowHeight());
    RHISetViewClear(viewID, 0x303030ff, 1.0f, 0);

    // Set new 3D rendering state
    globalRenderState = RenderState();
    globalRenderState.in3DMode = true;
    globalRenderState.inTextureMode = false;
    globalRenderState.camera = &camera;
    globalRenderState.viewMatrix = viewMtx;
    globalRenderState.projectionMatrix = projMtx;

    encoder = RHIBeginEncoder();
    BeginDebugDraw();
}

void EndDraw3D()
{
    if (!globalRenderStateStack.empty())
    {
        // Restore previous state
        globalRenderState = globalRenderStateStack.back();
        globalRenderStateStack.pop_back();

        // If returning to another 3D mode, update matrices
        if (globalRenderState.in3DMode && globalRenderState.camera)
        {
            float view[16];
            float proj[16];
            MatrixToFloat16(globalRenderState.viewMatrix, view);
            MatrixToFloat16(globalRenderState.projectionMatrix, proj);
            RHISetViewTransform(RHI_VIEW_3D, view, proj);
        }
    }
    else
    {
        // Reset to default state
        globalRenderState = RenderState();
    }
    EndDebugDraw();
    RHIEndEncoder(encoder);
}

void BeginTextureMode(RenderTexture &target)
{
    RHIDebugDrawEnd();

    // Push current state to stack
    globalRenderStateStack.push_back(globalRenderState);

    Camera *activeCamera = GetActiveCamera();

    // Set new texture rendering state
    globalRenderState = RenderState();
    globalRenderState.inTextureMode = true;
    globalRenderState.in3DMode = false;
    globalRenderState.target = &target;
    globalRenderState.viewID = target.viewID;
    globalRenderState.camera = activeCamera;

    // Set view to render to texture
    if (!target.IsValid())
    {
        DN_CORE_FATAL("RenderTexture invalid!");
    }
    else
    {
        RHISetViewFrameBuffer(target.viewID, target.frameBuffer);
        RHISetViewRect(target.viewID, 0, 0, target.width, target.height);
        RHISetViewClear(target.viewID, 0x00000000, 1.0f, 0);

        if (activeCamera)
        {
            Matrix viewMtx = GetCameraViewMatrix(activeCamera->GetImpl());
            Matrix projMtx = GetCameraProjectionMatrix(activeCamera->GetImpl());
            globalRenderState.viewMatrix = viewMtx;
            globalRenderState.projectionMatrix = projMtx;

            float view[16];
            float proj[16];
            MatrixToFloat16(viewMtx, view);
            MatrixToFloat16(projMtx, proj);
            RHISetViewTransform(target.viewID, view, proj);
        }
    }

    RHIDebugDrawBegin(globalRenderState.viewID, encoder);
}

void EndTextureMode()
{
    RHIDebugDrawEnd();
    if (!globalRenderStateStack.empty())
    {
        // Restore previous state
        globalRenderState = globalRenderStateStack.back();
        globalRenderStateStack.pop_back();
    }
    else
    {
        // Reset to default state
        globalRenderState = RenderState();
    }
    RHIDebugDrawBegin(globalRenderState.viewID, encoder);
}

void BeginDebugDraw()
{
    RHIDebugDrawBegin(globalRenderState.viewID, encoder);
}

void EndDebugDraw()
{
    RHIDebugDrawEnd();
}

void BeginEncoderFrame()
{
    encoder = RHIBeginEncoder();
    RHIDebugDrawBegin(globalRenderState.viewID, encoder);
}

void EndEncoderFrame()
{
    RHIDebugDrawEnd();
    RHIEndEncoder(encoder);
    RHIFrame();
    encoder = nullptr;
}

// ---------------------------------------------------------------------------
// Render queue
// ---------------------------------------------------------------------------

void QueueRender(RenderGeometryType::Type type)
{
    RHIViewId targetViewID = globalRenderState.viewID;
    RHIProgramHandle program = shaderProgramMap[DEFAULT_SHADERPROGRAM_UUID].program;

    GeometryBufferHandle buffers = GetGeometryBufferHandle(type);

    RHIEncoderSetVertexBuffer(encoder, 0, buffers.vbh);
    RHIEncoderSetIndexBuffer(encoder, buffers.ibh);
    RHIEncoderSubmit(encoder, targetViewID, program);
}

void QueueRender(RenderGeometryType::Type type, Vector3 position, Quaternion rotation, Vector3 size)
{
    Vector3 eulerRotation = QuaternionToEuler(rotation);

    RHIViewId targetViewID = globalRenderState.viewID;
    RHIProgramHandle program = shaderProgramMap[DEFAULT_SHADERPROGRAM_UUID].program;

    GeometryBufferHandle buffers = GetGeometryBufferHandle(type);
    if (!(buffers.vbh.IsValid() && buffers.ibh.IsValid()))
    {
        DN_CORE_WARN("Invalid geometry buffers submitted!");
        return;
    }

    float mtx[16];
    RHIComputeSRTMatrix(mtx, size.x, size.y, size.z, eulerRotation.x, eulerRotation.y, eulerRotation.z, position.x,
                        position.y, position.z);

    RHIEncoderSetTransform(encoder, mtx);
    RHIEncoderSetVertexBuffer(encoder, 0, buffers.vbh);
    RHIEncoderSetIndexBuffer(encoder, buffers.ibh);
    RHIEncoderSubmit(encoder, targetViewID, program);
}

void ExecuteRenderPipeline()
{
    if (globalRenderState.in3DMode && globalRenderState.camera)
    {
        globalRenderState.viewMatrix = GetCameraViewMatrix(globalRenderState.camera->GetImpl());
        globalRenderState.projectionMatrix = GetCameraProjectionMatrix(globalRenderState.camera->GetImpl());

        float view[16];
        float proj[16];
        MatrixToFloat16(globalRenderState.viewMatrix, view);
        MatrixToFloat16(globalRenderState.projectionMatrix, proj);
        RHISetViewTransform(globalRenderState.viewID, view, proj);
    }

    RHIFrame();
}

void EmptyRenderStack()
{
    globalRenderStateStack.clear();
    globalRenderState = RenderState();
    DN_CORE_INFO("Cleared render stack.");
}

// ---------------------------------------------------------------------------
// Drawing
// ---------------------------------------------------------------------------

void ClearBackground(Color color)
{
    RHISetViewClear(globalRenderState.viewID, color.PackedABGR(), 1.0f, 0);
    RHITouch(globalRenderState.viewID);
}

unsigned int DrawIMGUITexture(RenderTexture &renderTexture, Vector2 targetSize)
{
    ::ImTextureID img = (::ImTextureID)(uintptr_t)RHIGetTextureIdx(renderTexture.texture);
    ::ImVec2 size{targetSize.x, targetSize.y};
    ImGui::Image(img, size, ::ImVec2{1, 0}, ::ImVec2{0, 1});

    return RHIGetTextureIdx(renderTexture.texture);
}

void DestroyRenderTexture(RenderTexture &texture)
{
    texture.Destroy();
}

RenderState GetGlobalRenderState()
{
    return globalRenderState;
}

void DrawBox(Vector3 position, Quaternion rotation, Vector3 size)
{
    QueueRender(RenderGeometryType::BOX, position, rotation, size);
}

void DrawSphere(Vector3 position, Quaternion rotation, Vector3 size)
{
    QueueRender(RenderGeometryType::SPHERE, position, rotation, size);
}

void DrawSquare(Vector3 position, Quaternion rotation, Vector3 size)
{
    QueueRender(RenderGeometryType::PLANE, position, rotation, size);
}

void DrawGrid(float size)
{
    RHIDebugDrawGrid(0.0f, 0.0f, 0.0f, static_cast<uint32_t>(size), 1.0f);
}

void DrawDebugSphere(Vector3 pos, float radius)
{
    RHIDebugDrawOrb(pos.x, pos.y, pos.z, radius);
}

void DrawDebugCapsule(Vector3 from, Vector3 to, float radius)
{
    RHIDebugDrawCapsule(from.x, from.y, from.z, to.x, to.y, to.z, radius);
}

void DrawDebugBox(Vector3 min, Vector3 max)
{
    RHIDebugDrawAABB(min.x, min.y, min.z, max.x, max.y, max.z);
}

void DrawPlane(Vector3 size)
{
    Vector3 position = Vector3Zero();
    Quaternion rotation = QuaternionIdentity();
    QueueRender(RenderGeometryType::PLANE, position, rotation, size);
}

// ---------------------------------------------------------------------------
// Geometry buffer management
// ---------------------------------------------------------------------------

static void CreateGeometryBuffers()
{
    geometryBufferList.resize(RenderGeometryType::Count);

    /* Create BOX Buffers */
    {
        RHIVertexBufferHandle vbh =
            RHICreateVertexBuffer(BoxRenderGeometry::GetIdentityVertices(),
                                  static_cast<uint32_t>(BoxRenderGeometry::VertSize()) * sizeof(PosColorVertex));
        RHIIndexBufferHandle ibh = RHICreateIndexBuffer(BoxRenderGeometry::GetIdentityTriList(),
                                                        static_cast<uint32_t>(BoxRenderGeometry::TriSize()));
        geometryBufferList[RenderGeometryType::BOX] = GeometryBufferHandle(vbh, ibh);
    }

    /* Create PLANE Buffers */
    {
        RHIVertexBufferHandle vbh =
            RHICreateVertexBuffer(PlaneRenderGeometry::GetIdentityVertices(),
                                  static_cast<uint32_t>(PlaneRenderGeometry::VertSize()) * sizeof(PosColorVertex));
        RHIIndexBufferHandle ibh = RHICreateIndexBuffer(PlaneRenderGeometry::GetIdentityTriList(),
                                                        static_cast<uint32_t>(PlaneRenderGeometry::TriSize()));
        geometryBufferList[RenderGeometryType::PLANE] = GeometryBufferHandle(vbh, ibh);
    }

    /* Create SPHERE Buffers */
    {
        RHIVertexBufferHandle vbh =
            RHICreateVertexBuffer(SphereRenderGeometry::GetIdentityVertices(),
                                  static_cast<uint32_t>(SphereRenderGeometry::VertSize()) * sizeof(PosColorVertex));
        RHIIndexBufferHandle ibh = RHICreateIndexBuffer(SphereRenderGeometry::GetIdentityTriList(),
                                                        static_cast<uint32_t>(SphereRenderGeometry::TriSize()));
        geometryBufferList[RenderGeometryType::SPHERE] = GeometryBufferHandle(vbh, ibh);
    }
}

static GeometryBufferHandle GetGeometryBufferHandle(RenderGeometryType::Type type)
{
    switch (type)
    {
    case RenderGeometryType::BOX:
        return geometryBufferList[RenderGeometryType::BOX];
    case RenderGeometryType::PLANE:
        return geometryBufferList[RenderGeometryType::PLANE];
    case RenderGeometryType::SPHERE:
        return geometryBufferList[RenderGeometryType::SPHERE];
    default:
        DN_CORE_FATAL("Invalid render geometry handle!");
        return GeometryBufferHandle();
    }
}

} // namespace duin
