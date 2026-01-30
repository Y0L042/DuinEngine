#include "dnpch.h"
#include "Renderer.h"

#include "Shader.h"
#include "Camera.h"

#include <bx/math.h>
#include <debugdraw/debugdraw.h>
#include <external/imgui.h>

#include "Duin/Core/Debug/DNLog.h"
#include "Duin/Core/Application.h"
#include "Duin/Core/Maths/MathsModule.h"

namespace duin
{
struct BGFXBufferHandle
{
    UUID uuid;
    bgfx::VertexBufferHandle vbh = BGFX_INVALID_HANDLE;
    bgfx::IndexBufferHandle ibh = BGFX_INVALID_HANDLE;

    BGFXBufferHandle() : vbh(BGFX_INVALID_HANDLE), ibh(BGFX_INVALID_HANDLE)
    {
    }
    BGFXBufferHandle(bgfx::VertexBufferHandle vbh, bgfx::IndexBufferHandle ibh) : vbh(vbh), ibh(ibh)
    {
    }
};

static bool RENDER_CONTEXT_AVAILABLE = false; // Gets modified when imgui/bgfx is created, and destroyed
static UUID DEFAULT_SHADERPROGRAM_UUID;
static bgfx::VertexLayout pcvDecl;
static std::vector<BGFXBufferHandle> bgfxBufferList;
static std::unordered_map<UUID, ShaderProgram> shaderProgramMap;
static DebugDrawEncoder dde;
static DebugDrawState debugDrawState;
static bgfx::Encoder *encoder = nullptr;
static RenderState globalRenderState;
static std::vector<RenderState> globalRenderStateStack;

static void CreateGeometryBuffers();
static BGFXBufferHandle GetGeometryBufferHandle(RenderGeometryType::Type type);

void InitRenderer()
{
    pcvDecl.begin()
        .add(bgfx::Attrib::Position, 3, bgfx::AttribType::Float)
        .add(bgfx::Attrib::Color0, 4, bgfx::AttribType::Uint8, true)
        .end();

    // Load default shaders
    bgfx::ShaderHandle vsh =
        LoadShader("C:/Projects/CPP_Projects/Duin/Duin/src/Duin/Resources/shaders/dx11/vs_cubes.bin");
    bgfx::ShaderHandle fsh =
        LoadShader("C:/Projects/CPP_Projects/Duin/Duin/src/Duin/Resources/shaders/dx11/fs_cubes.bin");

    // Create default shaderprogram
    bgfx::ProgramHandle program = bgfx::createProgram(vsh, fsh, true);
    ShaderProgram shaderProgram(vsh, fsh, program);
    shaderProgramMap[shaderProgram.uuid] = shaderProgram;
    DEFAULT_SHADERPROGRAM_UUID = shaderProgram.uuid;

    CreateGeometryBuffers();

    ddInit();

    DN_CORE_INFO("Renderer initialised.");
}

void CleanRenderer()
{
    // TODO
    // Clean bgfx buffers
}

bool IsRenderContextAvailable()
{
    return RENDER_CONTEXT_AVAILABLE;
}

void SetRenderContextAvailable(bool available)
{
    RENDER_CONTEXT_AVAILABLE = available;
}

void BeginDraw3D(Camera &camera)
{
    // DN_CORE_INFO("BeginDraw3D");
    globalRenderStateStack.clear();
    const bgfx::ViewId viewID = 0;

    // Get camera matrices
    Matrix viewMtx = GetCameraViewMatrix(&camera);
    Matrix projMtx = GetCameraProjectionMatrix(&camera);

    // Set view transform
    static float view[16];
    static float proj[16];
    MatrixAsArray(viewMtx, view);
    MatrixAsArray(projMtx, proj);
    bgfx::setViewTransform(viewID, view, proj);
    bgfx::setViewRect(viewID, 0, 0, (uint16_t)GetWindowWidth(), (uint16_t)GetWindowHeight());
    bgfx::setViewClear(viewID, BGFX_CLEAR_COLOR | BGFX_CLEAR_DEPTH, 0x303030ff, 1.0f, 0);

    // Set new 3D rendering state
    globalRenderState.in3DMode = true;
    globalRenderState.inTextureMode = false;
    globalRenderState.camera = &camera;
    globalRenderState.viewMatrix = viewMtx;
    globalRenderState.projectionMatrix = projMtx;

    // Push current state to stack
    globalRenderStateStack.push_back(globalRenderState);

    encoder = bgfx::begin(false);
    BeginDebugDraw();
}

void EndDraw3D()
{
    // DN_CORE_INFO("EndDraw3D");
    if (!globalRenderStateStack.empty())
    {
        // Restore previous state
        globalRenderState = globalRenderStateStack.back();
        globalRenderStateStack.pop_back();

        // If returning to another 3D mode, update matrices
        if (globalRenderState.in3DMode && globalRenderState.camera)
        {
            static float view[16];
            static float proj[16];
            MatrixAsArray(globalRenderState.viewMatrix, view);
            MatrixAsArray(globalRenderState.projectionMatrix, proj);
            bgfx::setViewTransform(0, view, proj);
        }
    }
    else
    {
        // Reset to default state
        globalRenderState = RenderState();
    }
    EndDebugDraw();
    bgfx::end(encoder);
}

void BeginTextureMode(RenderTexture &target)
{
    // Push current state to stack
    globalRenderStateStack.push_back(globalRenderState);

    // Set new texture rendering state
    globalRenderState.inTextureMode = true;
    globalRenderState.in3DMode = false;
    globalRenderState.target = &target;
    globalRenderState.viewID = target.viewID;

    // Set view to render to texture
    if (!target.IsValid())
    {
        DN_CORE_FATAL("RenderTexture invalid!");
    }
    else
    {
        bgfx::setViewFrameBuffer(target.viewID, target.frameBuffer);
        bgfx::setViewRect(target.viewID, 0, 0, target.width, target.height); // Clear render texture
        bgfx::setViewClear(target.viewID, BGFX_CLEAR_COLOR | BGFX_CLEAR_DEPTH, 0x00000000, 1.0f, 0);
    }
}

void EndTextureMode()
{
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
}

void BeginDebugDraw()
{
    dde.begin(0, true, encoder);
    // DN_CORE_TRACE("Debug draw started");
}

void EndDebugDraw()
{
    dde.end();
    // DN_CORE_TRACE("Debug draw ended");
}

void QueueRender(RenderGeometryType::Type type)
{
    // DN_CORE_INFO("Shape queued for rendering");

    bgfx::VertexBufferHandle vbh = BGFX_INVALID_HANDLE;
    bgfx::IndexBufferHandle ibh = BGFX_INVALID_HANDLE;
    bgfx::ProgramHandle program = shaderProgramMap[DEFAULT_SHADERPROGRAM_UUID].program;

    BGFXBufferHandle buffers = GetGeometryBufferHandle(type);
    vbh = buffers.vbh;
    ibh = buffers.ibh;

    // bgfx::setVertexBuffer(0, vbh);
    // bgfx::setIndexBuffer(ibh);
    // bgfx::submit(0, program);

    encoder->setVertexBuffer(0, vbh);
    encoder->setIndexBuffer(ibh);
    encoder->submit(0, program);
}

void QueueRender(RenderGeometryType::Type type, Vector3 position, Quaternion rotation, Vector3 size)
{
    // DN_CORE_INFO("Shape queued for rendering");

    Vector3 eulerRotation = QuaternionToEuler(rotation);

    bgfx::VertexBufferHandle vbh = BGFX_INVALID_HANDLE;
    bgfx::IndexBufferHandle ibh = BGFX_INVALID_HANDLE;
    bgfx::ProgramHandle program = shaderProgramMap[DEFAULT_SHADERPROGRAM_UUID].program;

    BGFXBufferHandle buffers = GetGeometryBufferHandle(type);
    vbh = buffers.vbh;
    ibh = buffers.ibh;
    if (!(bgfx::isValid(vbh) && bgfx::isValid(ibh)))
    {
        DN_CORE_WARN("Invalid geometry buffers submitted!");
        return;
    }

    float mtx[16];
    bx::mtxSRT(mtx, size.x, size.y, size.z, eulerRotation.x, eulerRotation.y, eulerRotation.z, position.x, position.y,
               position.z);

    // bgfx::setTransform(mtx);
    // bgfx::setVertexBuffer(0, vbh);
    // bgfx::setIndexBuffer(ibh);
    // bgfx::submit(0, program);

    encoder->setTransform(mtx);
    encoder->setVertexBuffer(0, vbh);
    encoder->setIndexBuffer(ibh);
    encoder->submit(0, program);
}

void ExecuteRenderPipeline()
{
    // Camera *activeCamera = GetActiveCamera();
    // if (activeCamera && activeCamera->IsValid()) {
    //     GetBGFXMatrix(activeCamera);
    // }

    if (globalRenderState.in3DMode && globalRenderState.camera)
    {
        globalRenderState.viewMatrix = GetCameraViewMatrix(globalRenderState.camera);
        globalRenderState.projectionMatrix = GetCameraProjectionMatrix(globalRenderState.camera);

        static float view[16];
        static float proj[16];
        MatrixAsArray(globalRenderState.viewMatrix, view);
        MatrixAsArray(globalRenderState.projectionMatrix, proj);
        bgfx::setViewTransform(0, view, proj);
    }

    bgfx::frame();
}

void EmptyRenderStack()
{
    globalRenderStateStack.clear();
    globalRenderState = RenderState();
    DN_CORE_INFO("Cleared render stack.");
}

void ClearBackground(Color color, int win)
{
    bgfx::setViewClear(win, BGFX_CLEAR_COLOR | BGFX_CLEAR_DEPTH, color.PackedABGR(), 1.0f, 0);
    bgfx::touch(win);
}

unsigned int DrawIMGUITexture(RenderTexture &renderTexture, Vector2 targetSize)
{
    ::ImTextureID img = (::ImTextureID)(uintptr_t)renderTexture.texture.idx;
    ::ImVec2 size{targetSize.x, targetSize.y};
    ImGui::Image(img, size, ::ImVec2{1, 0}, ::ImVec2{0, 1});

    return 0;
}

void DestroyRenderTexture(RenderTexture &texture)
{
    texture.Destroy();
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
    dde.drawGrid(Axis::Y, {0.0f, 0.0f, 0.0f}, static_cast<uint32_t>(size), 1.0f);
}

void DrawDebugSphere(Vector3 pos, float radius)
{
    dde.drawOrb(pos.x, pos.y, pos.z, radius);
}

void DrawPlane(Vector3 size)
{
    Vector3 position = Vector3Zero();
    Quaternion rotation = QuaternionIdentity();
    QueueRender(RenderGeometryType::PLANE, position, rotation, size);
}

static void CreateGeometryBuffers()
{
    bgfxBufferList.resize(RenderGeometryType::Count);

    bgfx::VertexBufferHandle vbh = BGFX_INVALID_HANDLE;
    bgfx::IndexBufferHandle ibh = BGFX_INVALID_HANDLE;

    /* Create BOX Buffers */
    vbh = BGFX_INVALID_HANDLE;
    ibh = BGFX_INVALID_HANDLE;
    vbh = bgfx::createVertexBuffer(
        bgfx::makeRef(BoxRenderGeometry::GetIdentityVertices(),
                      static_cast<uint32_t>(BoxRenderGeometry::VertSize()) * sizeof(PosColorVertex)),
        pcvDecl);
    ibh =
        bgfx::createIndexBuffer(bgfx::makeRef(BoxRenderGeometry::GetIdentityTriList(),
                                              static_cast<uint32_t>(BoxRenderGeometry::TriSize()) * sizeof(uint16_t)));
    bgfxBufferList[RenderGeometryType::BOX] = BGFXBufferHandle(vbh, ibh);

    /* Create PLANE Buffers */
    vbh = BGFX_INVALID_HANDLE;
    ibh = BGFX_INVALID_HANDLE;
    vbh = bgfx::createVertexBuffer(
        bgfx::makeRef(PlaneRenderGeometry::GetIdentityVertices(),
                      static_cast<uint32_t>(PlaneRenderGeometry::VertSize()) * sizeof(PosColorVertex)),
        pcvDecl);
    ibh = bgfx::createIndexBuffer(
        bgfx::makeRef(PlaneRenderGeometry::GetIdentityTriList(),
                      static_cast<uint32_t>(PlaneRenderGeometry::TriSize()) * sizeof(uint16_t)));
    bgfxBufferList[RenderGeometryType::PLANE] = BGFXBufferHandle(vbh, ibh);

    /* Create SPHERE Buffers */
    vbh = BGFX_INVALID_HANDLE;
    ibh = BGFX_INVALID_HANDLE;
    vbh = bgfx::createVertexBuffer(
        bgfx::makeRef(SphereRenderGeometry::GetIdentityVertices(),
                      static_cast<uint32_t>(SphereRenderGeometry::VertSize()) * sizeof(PosColorVertex)),
        pcvDecl);
    ibh = bgfx::createIndexBuffer(
        bgfx::makeRef(SphereRenderGeometry::GetIdentityTriList(),
                      static_cast<uint32_t>(SphereRenderGeometry::TriSize()) * sizeof(uint16_t)));
    bgfxBufferList[RenderGeometryType::SPHERE] = BGFXBufferHandle(vbh, ibh);
}

static BGFXBufferHandle GetGeometryBufferHandle(RenderGeometryType::Type type)
{
    switch (type)
    {
    case RenderGeometryType::BOX:
        return bgfxBufferList[RenderGeometryType::BOX];
        break;
    case RenderGeometryType::PLANE:
        return bgfxBufferList[RenderGeometryType::PLANE];
        break;
    case RenderGeometryType::SPHERE:
        return bgfxBufferList[RenderGeometryType::SPHERE];
        break;
    default:
        DN_CORE_FATAL("Invalid render geometry handle!");
        break;
    }
}
} // namespace duin
