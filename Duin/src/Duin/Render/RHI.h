#pragma once

#include <cstdint>
#include <Windows.h>
#include <functional>
#include <ctype.h>

struct ImDrawData; // global ImGui type forward declaration

namespace duin
{
// clang-format off

// ---------------------------------------------------------------------------
// Constants
// ---------------------------------------------------------------------------

extern const uint32_t RHI_RESET_VSYNC;

// ---------------------------------------------------------------------------
// Opaque GPU resource handles
// ---------------------------------------------------------------------------

struct RHITextureHandle
{
    uint16_t idx = UINT16_MAX;
    bool IsValid() const { return idx != UINT16_MAX; }
};

struct RHIFrameBufferHandle
{
    uint16_t idx = UINT16_MAX;
    bool IsValid() const { return idx != UINT16_MAX; }
};

struct RHIShaderHandle
{
    uint16_t idx = UINT16_MAX;
    bool IsValid() const { return idx != UINT16_MAX; }
};

struct RHIProgramHandle
{
    uint16_t idx = UINT16_MAX;
    bool IsValid() const { return idx != UINT16_MAX; }
};

struct RHIVertexBufferHandle
{
    uint16_t idx = UINT16_MAX;
    bool IsValid() const { return idx != UINT16_MAX; }
};

struct RHIIndexBufferHandle
{
    uint16_t idx = UINT16_MAX;
    bool IsValid() const { return idx != UINT16_MAX; }
};

using RHIViewId = uint16_t;

// Opaque encoder -- never defined in this header; internally a bgfx::Encoder*.
struct RHIEncoder;

// ---------------------------------------------------------------------------
// Constants
// ---------------------------------------------------------------------------

static constexpr RHIViewId RHI_VIEW_3D = 0;
static constexpr RHIViewId RHI_VIEW_RENDERTEXTURE = 1;
static constexpr RHIViewId RHI_VIEW_IMGUI = 255;

// ---------------------------------------------------------------------------
// Setup/Shutdown
// ---------------------------------------------------------------------------

void RHIStart(RHIViewId viewId, uint32_t width, uint32_t height, 
              std::function<HWND(void)> renderThreadCapture,
              bool headless = false);
void RHIClose();

// ---------------------------------------------------------------------------
// Lifecycle
// ---------------------------------------------------------------------------

void RHIInit();
void RHIShutdown();
void RHIFrame();

// ---------------------------------------------------------------------------
// Shaders
// ---------------------------------------------------------------------------

RHIShaderHandle  RHILoadShader(const char *path);
RHIProgramHandle RHICreateProgram(RHIShaderHandle vsh, RHIShaderHandle fsh, bool destroyShaders);
void             RHIDestroyProgram(RHIProgramHandle handle);

// ---------------------------------------------------------------------------
// Vertex / Index Buffers
// ---------------------------------------------------------------------------

RHIVertexBufferHandle RHICreateVertexBuffer(const void *data, uint32_t sizeBytes);
RHIIndexBufferHandle  RHICreateIndexBuffer(const uint16_t *data, uint32_t count);
void                  RHIDestroyVertexBuffer(RHIVertexBufferHandle handle);
void                  RHIDestroyIndexBuffer(RHIIndexBufferHandle handle);

// ---------------------------------------------------------------------------
// Textures / Framebuffers
// ---------------------------------------------------------------------------

RHITextureHandle     RHICreateTexture2D(uint16_t width, uint16_t height);
RHIFrameBufferHandle RHICreateFrameBuffer(RHITextureHandle texture);
void                 RHIDestroyTexture(RHITextureHandle handle);
void                 RHIDestroyFrameBuffer(RHIFrameBufferHandle handle);
uint16_t             RHIGetTextureIdx(RHITextureHandle handle);

// ---------------------------------------------------------------------------
// View Setup
// ---------------------------------------------------------------------------

void RHISetViewTransform(RHIViewId viewId, const float *view, const float *proj);
void RHISetViewRect(RHIViewId viewId, uint16_t x, uint16_t y, uint16_t width, uint16_t height);
void RHISetViewClear(RHIViewId viewId, uint32_t rgba, float depth, uint8_t stencil);
void RHISetViewFrameBuffer(RHIViewId viewId, RHIFrameBufferHandle handle);
void RHITouch(RHIViewId viewId);

// ---------------------------------------------------------------------------
// Encoder (command submission)
// ---------------------------------------------------------------------------

RHIEncoder *RHIBeginEncoder();
void        RHIEndEncoder(RHIEncoder *enc);

void RHIEncoderSetTransform(RHIEncoder *enc, const float *mtx);
void RHIEncoderSetVertexBuffer(RHIEncoder *enc, uint8_t stream, RHIVertexBufferHandle handle);
void RHIEncoderSetIndexBuffer(RHIEncoder *enc, RHIIndexBufferHandle handle);
void RHIEncoderSubmit(RHIEncoder *enc, RHIViewId viewId, RHIProgramHandle program);

// ---------------------------------------------------------------------------
// Matrix Math (wraps bx:: functions that depend on RHI backend caps)
// ---------------------------------------------------------------------------

void RHIComputeViewMatrix(float *out16,
                          float eyeX, float eyeY, float eyeZ,
                          float atX, float atY, float atZ,
                          float upX, float upY, float upZ);

void RHIComputeProjMatrix(float *out16,
                          float fovy, float aspect, float nearPlane, float farPlane);

void RHIComputeSRTMatrix(float *out16,
                         float sx, float sy, float sz,
                         float rx, float ry, float rz,
                         float tx, float ty, float tz);

// ---------------------------------------------------------------------------
// Debug Draw
// ---------------------------------------------------------------------------

void RHIDebugDrawInit();
void RHIDebugDrawShutdown();
void RHIDebugDrawBegin(RHIViewId viewId, RHIEncoder *enc);
void RHIDebugDrawEnd();

void RHIDebugDrawGrid(float originX, float originY, float originZ, uint32_t size, float step);
void RHIDebugDrawOrb(float x, float y, float z, float radius);
void RHIDebugDrawCapsule(float fromX, float fromY, float fromZ,
                         float toX, float toY, float toZ, float radius);
void RHIDebugDrawAABB(float minX, float minY, float minZ,
                      float maxX, float maxY, float maxZ);

// ---------------------------------------------------------------------------
// Rendering (bgfx::reset wrapper)
// ---------------------------------------------------------------------------

void RHIReset(uint32_t width, uint32_t height, uint32_t flags);

// ---------------------------------------------------------------------------
// ImGui Backend Wrappers
// ---------------------------------------------------------------------------

void RHIImGuiInit(uint16_t viewId);
void RHIImGuiShutdown();
void RHIImGuiNewFrame();
void RHIImGuiRenderDrawLists(::ImDrawData *drawData);

// clang-format on

} // namespace duin
