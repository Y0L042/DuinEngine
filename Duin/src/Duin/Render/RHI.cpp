#include "dnpch.h"
#include "RHI.h"

#include <bgfx/bgfx.h>
#include <bgfx/platform.h>
#include <bx/math.h>
#include <debugdraw/debugdraw.h>
#include "external/backends/imgui_impl_bgfx.h"
#include "external/imgui.h"

#include "Duin/Core/Debug/DNLog.h"

#include <cstdio>
#include <cstring>
#include <Windows.h>
#include <cstdint>
#include <bgfx/defines.h>
#include <bx/bounds.h>
#include <SDL3/SDL_properties.h>
#include <SDL3/SDL_video.h>
#include <functional>

namespace duin
{

// ---------------------------------------------------------------------------
// Constants
// ---------------------------------------------------------------------------

const uint32_t RHI_RESET_VSYNC = BGFX_RESET_VSYNC;

// ---------------------------------------------------------------------------
// Internal helpers
// ---------------------------------------------------------------------------

static bgfx::VertexLayout s_pcvLayout;
static DebugDrawEncoder s_dde;

// Handle conversions -- both sides store a uint16_t idx, so direct copy.

static bgfx::TextureHandle ToBgfx(RHITextureHandle h)
{
    bgfx::TextureHandle bh;
    bh.idx = h.idx;
    return bh;
}

static bgfx::FrameBufferHandle ToBgfx(RHIFrameBufferHandle h)
{
    bgfx::FrameBufferHandle bh;
    bh.idx = h.idx;
    return bh;
}

static bgfx::ShaderHandle ToBgfx(RHIShaderHandle h)
{
    bgfx::ShaderHandle bh;
    bh.idx = h.idx;
    return bh;
}

static bgfx::ProgramHandle ToBgfx(RHIProgramHandle h)
{
    bgfx::ProgramHandle bh;
    bh.idx = h.idx;
    return bh;
}

static bgfx::VertexBufferHandle ToBgfx(RHIVertexBufferHandle h)
{
    bgfx::VertexBufferHandle bh;
    bh.idx = h.idx;
    return bh;
}

static bgfx::IndexBufferHandle ToBgfx(RHIIndexBufferHandle h)
{
    bgfx::IndexBufferHandle bh;
    bh.idx = h.idx;
    return bh;
}

static RHITextureHandle FromBgfx(bgfx::TextureHandle h)
{
    RHITextureHandle rh;
    rh.idx = h.idx;
    return rh;
}

static RHIFrameBufferHandle FromBgfx(bgfx::FrameBufferHandle h)
{
    RHIFrameBufferHandle rh;
    rh.idx = h.idx;
    return rh;
}

static RHIShaderHandle FromBgfx(bgfx::ShaderHandle h)
{
    RHIShaderHandle rh;
    rh.idx = h.idx;
    return rh;
}

static RHIProgramHandle FromBgfx(bgfx::ProgramHandle h)
{
    RHIProgramHandle rh;
    rh.idx = h.idx;
    return rh;
}

static RHIVertexBufferHandle FromBgfx(bgfx::VertexBufferHandle h)
{
    RHIVertexBufferHandle rh;
    rh.idx = h.idx;
    return rh;
}

static RHIIndexBufferHandle FromBgfx(bgfx::IndexBufferHandle h)
{
    RHIIndexBufferHandle rh;
    rh.idx = h.idx;
    return rh;
}

static bgfx::Encoder *ToEncoder(RHIEncoder *enc)
{
    return reinterpret_cast<bgfx::Encoder *>(enc);
}

static RHIEncoder *FromEncoder(bgfx::Encoder *enc)
{
    return reinterpret_cast<RHIEncoder *>(enc);
}

// Row-major Matrix (raymath) to column-major float[16] for bgfx.
// Mirrors the old MatrixAsArray from BGFXConversions.h.
static void MatrixAsArray(const float *m, float *r)
{
    // Input: m0  m1  m2  m3    (row 0)
    //        m4  m5  m6  m7    (row 1)
    //        m8  m9  m10 m11   (row 2)
    //        m12 m13 m14 m15   (row 3)
    // Output: column-major (transpose)
    r[0] = m[0];
    r[1] = m[4];
    r[2] = m[8];
    r[3] = m[12];

    r[4] = m[1];
    r[5] = m[5];
    r[6] = m[9];
    r[7] = m[13];

    r[8] = m[2];
    r[9] = m[6];
    r[10] = m[10];
    r[11] = m[14];

    r[12] = m[3];
    r[13] = m[7];
    r[14] = m[11];
    r[15] = m[15];
}

// ---------------------------------------------------------------------------
// Setup/Shutdown
// ---------------------------------------------------------------------------

void RHIStart(RHIViewId viewId, uint32_t width, uint32_t height, std::function<HWND(void)> renderThreadCapture, bool headless)
{
    bgfx::Init bgfxInit;
    bgfxInit.type = bgfx::RendererType::Count; // Automatically choose a renderer

    if (headless)
    {
        bgfxInit.resolution.width = 0;
        bgfxInit.resolution.height = 0;
        bgfxInit.resolution.reset = BGFX_RESET_NONE;
        bgfxInit.platformData.nwh = nullptr;
    }
    else
    {
        bgfx::renderFrame(); // Claim render thread before bgfx::init() spawns one
        ::HWND hwnd = renderThreadCapture();
        if (!hwnd)
        {
            DN_CORE_FATAL("SDL3 window handle not found!");
        }
        bgfxInit.resolution.width = width;
        bgfxInit.resolution.height = height;
        bgfxInit.resolution.reset = BGFX_RESET_VSYNC;
        bgfxInit.platformData.nwh = hwnd;
    }

    bgfx::init(bgfxInit);

    if (!headless)
    {
        bgfx::setViewClear(viewId, BGFX_CLEAR_COLOR | BGFX_CLEAR_DEPTH, 0x443355FF, 1.0f, 0);
        bgfx::setViewRect(viewId, 0, 0, width, height);
    }

    DN_CORE_INFO("RHI [bgfx] started (headless={}).", headless);
}

void RHIClose()
{
    bgfx::shutdown();

    DN_CORE_INFO("RHI [bgfx] closed.");
}

// ---------------------------------------------------------------------------
// Lifecycle
// ---------------------------------------------------------------------------

void RHIInit()
{
    s_pcvLayout.begin()
        .add(bgfx::Attrib::Position, 3, bgfx::AttribType::Float)
        .add(bgfx::Attrib::Color0, 4, bgfx::AttribType::Uint8, true)
        .end();

    RHIDebugDrawInit();
}

void RHIShutdown()
{
    s_dde.end();
    RHIDebugDrawShutdown();

    // Force-reconstruct s_dde so its internal state is clean for next init.
    s_dde.~DebugDrawEncoder();
    new (&s_dde) DebugDrawEncoder();
}

void RHIFrame()
{
    bgfx::frame();
}

// ---------------------------------------------------------------------------
// Shaders
// ---------------------------------------------------------------------------

RHIShaderHandle RHILoadShader(const char *path)
{
    FILE *file = fopen(path, "rb");
    if (!file)
    {
        DN_CORE_ERROR("Failed to open shader file: {}", path);
        RHIShaderHandle invalid;
        return invalid;
    }

    fseek(file, 0, SEEK_END);
    long fileSize = ftell(file);
    fseek(file, 0, SEEK_SET);

    const bgfx::Memory *mem = bgfx::alloc(fileSize + 1);
    fread(mem->data, 1, fileSize, file);
    mem->data[mem->size - 1] = '\0';
    fclose(file);

    return FromBgfx(bgfx::createShader(mem));
}

RHIProgramHandle RHICreateProgram(RHIShaderHandle vsh, RHIShaderHandle fsh, bool destroyShaders)
{
    return FromBgfx(bgfx::createProgram(ToBgfx(vsh), ToBgfx(fsh), destroyShaders));
}

void RHIDestroyProgram(RHIProgramHandle handle)
{
    bgfx::ProgramHandle bh = ToBgfx(handle);
    if (bgfx::isValid(bh))
    {
        bgfx::destroy(bh);
    }
}

// ---------------------------------------------------------------------------
// Vertex / Index Buffers
// ---------------------------------------------------------------------------

RHIVertexBufferHandle RHICreateVertexBuffer(const void *data, uint32_t sizeBytes)
{
    bgfx::VertexBufferHandle vbh = bgfx::createVertexBuffer(bgfx::makeRef(data, sizeBytes), s_pcvLayout);
    return FromBgfx(vbh);
}

RHIIndexBufferHandle RHICreateIndexBuffer(const uint16_t *data, uint32_t count)
{
    bgfx::IndexBufferHandle ibh = bgfx::createIndexBuffer(bgfx::makeRef(data, count * sizeof(uint16_t)));
    return FromBgfx(ibh);
}

void RHIDestroyVertexBuffer(RHIVertexBufferHandle handle)
{
    bgfx::VertexBufferHandle bh = ToBgfx(handle);
    if (bgfx::isValid(bh))
    {
        bgfx::destroy(bh);
    }
}

void RHIDestroyIndexBuffer(RHIIndexBufferHandle handle)
{
    bgfx::IndexBufferHandle bh = ToBgfx(handle);
    if (bgfx::isValid(bh))
    {
        bgfx::destroy(bh);
    }
}

// ---------------------------------------------------------------------------
// Textures / Framebuffers
// ---------------------------------------------------------------------------

RHITextureHandle RHICreateTexture2D(uint16_t width, uint16_t height)
{
    width = (width > 0) ? width : 1;
    height = (height > 0) ? height : 1;

    bgfx::TextureHandle tex = bgfx::createTexture2D(width, height,
                                                    false, // mipmaps
                                                    1,     // num layers
                                                    bgfx::TextureFormat::RGBA8,
                                                    BGFX_TEXTURE_RT | BGFX_SAMPLER_MIN_POINT | BGFX_SAMPLER_MAG_POINT |
                                                        BGFX_SAMPLER_MIP_POINT);

    return FromBgfx(tex);
}

RHIFrameBufferHandle RHICreateFrameBuffer(RHITextureHandle texture)
{
    bgfx::TextureHandle tex = ToBgfx(texture);
    bgfx::FrameBufferHandle fb = bgfx::createFrameBuffer(1, &tex);
    return FromBgfx(fb);
}

void RHIDestroyTexture(RHITextureHandle handle)
{
    bgfx::TextureHandle bh = ToBgfx(handle);
    if (bgfx::isValid(bh))
    {
        bgfx::destroy(bh);
    }
}

void RHIDestroyFrameBuffer(RHIFrameBufferHandle handle)
{
    bgfx::FrameBufferHandle bh = ToBgfx(handle);
    if (bgfx::isValid(bh))
    {
        bgfx::destroy(bh);
    }
}

uint16_t RHIGetTextureIdx(RHITextureHandle handle)
{
    return handle.idx;
}

// ---------------------------------------------------------------------------
// View Setup
// ---------------------------------------------------------------------------

void RHISetViewTransform(RHIViewId viewId, const float *view, const float *proj)
{
    bgfx::setViewTransform(viewId, view, proj);
}

void RHISetViewRect(RHIViewId viewId, uint16_t x, uint16_t y, uint16_t width, uint16_t height)
{
    bgfx::setViewRect(viewId, x, y, width, height);
}

void RHISetViewClear(RHIViewId viewId, uint32_t rgba, float depth, uint8_t stencil)
{
    bgfx::setViewClear(viewId, BGFX_CLEAR_COLOR | BGFX_CLEAR_DEPTH, rgba, depth, stencil);
}

void RHISetViewFrameBuffer(RHIViewId viewId, RHIFrameBufferHandle handle)
{
    bgfx::setViewFrameBuffer(viewId, ToBgfx(handle));
}

void RHITouch(RHIViewId viewId)
{
    bgfx::touch(viewId);
}

// ---------------------------------------------------------------------------
// Encoder
// ---------------------------------------------------------------------------

RHIEncoder *RHIBeginEncoder()
{
    return FromEncoder(bgfx::begin(false));
}

void RHIEndEncoder(RHIEncoder *enc)
{
    if (enc)
    {
        bgfx::end(ToEncoder(enc));
    }
}

void RHIEncoderSetTransform(RHIEncoder *enc, const float *mtx)
{
    ToEncoder(enc)->setTransform(mtx);
}

void RHIEncoderSetVertexBuffer(RHIEncoder *enc, uint8_t stream, RHIVertexBufferHandle handle)
{
    ToEncoder(enc)->setVertexBuffer(stream, ToBgfx(handle));
}

void RHIEncoderSetIndexBuffer(RHIEncoder *enc, RHIIndexBufferHandle handle)
{
    ToEncoder(enc)->setIndexBuffer(ToBgfx(handle));
}

void RHIEncoderSubmit(RHIEncoder *enc, RHIViewId viewId, RHIProgramHandle program)
{
    ToEncoder(enc)->submit(viewId, ToBgfx(program));
}

// ---------------------------------------------------------------------------
// Matrix Math
// ---------------------------------------------------------------------------

void RHIComputeViewMatrix(float *out16, float eyeX, float eyeY, float eyeZ, float atX, float atY, float atZ, float upX,
                          float upY, float upZ)
{
    bx::Vec3 eye = {eyeX, eyeY, eyeZ};
    bx::Vec3 at = {atX, atY, atZ};
    bx::Vec3 up = {upX, upY, upZ};
    bx::mtxLookAt(out16, eye, at, up);
}

void RHIComputeProjMatrix(float *out16, float fovy, float aspect, float nearPlane, float farPlane)
{
    bx::mtxProj(out16, fovy, aspect, nearPlane, farPlane, bgfx::getCaps()->homogeneousDepth);
}

void RHIComputeSRTMatrix(float *out16, float sx, float sy, float sz, float rx, float ry, float rz, float tx, float ty,
                         float tz)
{
    bx::mtxSRT(out16, sx, sy, sz, rx, ry, rz, tx, ty, tz);
}

// ---------------------------------------------------------------------------
// Debug Draw
// ---------------------------------------------------------------------------

void RHIDebugDrawInit()
{
    ddInit();
}

void RHIDebugDrawShutdown()
{
    ddShutdown();
}

void RHIDebugDrawBegin(RHIViewId viewId, RHIEncoder *enc)
{
    s_dde.begin(viewId, true, ToEncoder(enc));
}

void RHIDebugDrawEnd()
{
    s_dde.end();
}

void RHIDebugDrawGrid(float originX, float originY, float originZ, uint32_t size, float step)
{
    s_dde.drawGrid(Axis::Y, {originX, originY, originZ}, size, step);
}

void RHIDebugDrawOrb(float x, float y, float z, float radius)
{
    s_dde.drawOrb(x, y, z, radius);
}

void RHIDebugDrawCapsule(float fromX, float fromY, float fromZ, float toX, float toY, float toZ, float radius)
{
    s_dde.drawCapsule({fromX, fromY, fromZ}, {toX, toY, toZ}, radius);
}

void RHIDebugDrawAABB(float minX, float minY, float minZ, float maxX, float maxY, float maxZ)
{
    bx::Aabb aabb;
    aabb.min = {minX, minY, minZ};
    aabb.max = {maxX, maxY, maxZ};
    s_dde.draw(aabb);
}

void RHIDebugDrawTriangle(const Vector3 c1, const Vector3 c2, const Vector3 c3)
{
    s_dde.draw({{c1.x, c1.y, c1.z}, {c2.x, c2.y, c2.z}, {c3.x, c3.y, c3.z}});
}

void RHIDebugDrawSetColor(uint32_t abgr)
{
    s_dde.setColor(abgr);
}

void RHIDebugDrawLine(float fromX, float fromY, float fromZ,
                      float toX,   float toY,   float toZ)
{
    s_dde.moveTo(fromX, fromY, fromZ);
    s_dde.lineTo(toX,   toY,   toZ);
}

// ---------------------------------------------------------------------------
// Rendering (bgfx::reset wrapper)
// ---------------------------------------------------------------------------

void RHIReset(uint32_t width, uint32_t height, uint32_t flags)
{
    bgfx::reset(width, height, flags);
}

// ---------------------------------------------------------------------------
// ImGui Backend Wrappers
// ---------------------------------------------------------------------------

void RHIImGuiInit(uint16_t viewId)
{
    ImGui_Implbgfx_Init(viewId);
}

void RHIImGuiShutdown()
{
    ImGui_Implbgfx_Shutdown();
}

void RHIImGuiNewFrame()
{
    ImGui_Implbgfx_NewFrame();
}

void RHIImGuiRenderDrawLists(ImDrawData *drawData)
{
    ImGui_Implbgfx_RenderDrawLists((::ImDrawData *)drawData);
}

} // namespace duin
