#pragma once

#include <cstdint>
#include <Windows.h>
#include <functional>
#include <ctype.h>
#include "Duin/Core/Maths/DuinMaths.h"

struct ImDrawData; // global ImGui type forward declaration

namespace duin
{
// clang-format off

// ---------------------------------------------------------------------------
// Constants
// ---------------------------------------------------------------------------

extern const uint32_t RHI_RESET_VSYNC;

struct Vertex
{
    float x, y, z;
};

struct PosColorVertex : public Vertex
{
    uint32_t abgr;
};

// Mirrors bgfx::Attrib::Enum. Order must match; asserted in RHI.cpp.
enum class RHIAttrib : uint8_t
{
    Position = 0,
    Normal,
    Tangent,
    Bitangent,
    Color0,
    Color1,
    Color2,
    Color3,
    Indices,
    Weight,
    TexCoord0,
    TexCoord1,
    TexCoord2,
    TexCoord3,
    TexCoord4,
    TexCoord5,
    TexCoord6,
    TexCoord7,
    Count
};

// Mirrors bgfx::AttribType::Enum. Order must match; asserted in RHI.cpp.
enum class RHIAttribType : uint8_t
{
    Int8 = 0,
    Uint8,
    Uint10,
    Int16,
    Uint16,
    Half,
    Float,
    Count
};

static constexpr uint8_t RHI_MAX_VERTEX_ATTRIBS = (uint8_t)RHIAttrib::Count;

// ---------------------------------------------------------------------------
// Vertex layout descriptor
// ---------------------------------------------------------------------------

struct RHIVertexLayout
{
    RHIAttrib     attrib[RHI_MAX_VERTEX_ATTRIBS];
    uint8_t       num[RHI_MAX_VERTEX_ATTRIBS];
    RHIAttribType type[RHI_MAX_VERTEX_ATTRIBS];
    bool          normalized[RHI_MAX_VERTEX_ATTRIBS];
    bool          asInt[RHI_MAX_VERTEX_ATTRIBS];
    uint8_t       skipBytes[RHI_MAX_VERTEX_ATTRIBS]; // >0 marks entry as padding
    uint8_t       count = 0;

    // Add an attribute. `num` is 1, 2, 3 or 4. When `normalized`, a fixed-point
    // type (e.g. Uint8 in 0-255) reads as 0.0-1.0 in the vertex shader.
    // `asInt` is the packing rule for Uint8/Int16 in vertex pack/unpack.
    RHIVertexLayout &Add(RHIAttrib attrib, uint8_t num, RHIAttribType type,
                         bool normalized = false, bool asInt = false);

    // Skip `bytes` of padding in the vertex stream.
    RHIVertexLayout &Skip(uint8_t bytes);

    // Decode a previously added attribute. Returns false if not present.
    bool Decode(RHIAttrib attrib, uint8_t &num, RHIAttribType &type,
                bool &normalized, bool &asInt) const;

    bool     Has(RHIAttrib attrib) const;
    uint16_t GetOffset(RHIAttrib attrib) const; // UINT16_MAX if not present
    uint16_t GetStride() const;
    uint32_t GetSize(uint32_t numVertices) const;
};

// Size in bytes of one attribute element of the given type and count.
uint16_t RHIAttribTypeSize(RHIAttribType type, uint8_t num);

// Built-in layout matching PosColorVertex (position 3xFloat + color 4xUint8N).
const RHIVertexLayout &RHIGetPosColorLayout();

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

// Omitting `layout` defaults to RHIGetPosColorLayout().
RHIVertexBufferHandle RHICreateVertexBuffer(const void *data, uint32_t sizeBytes);
RHIVertexBufferHandle RHICreateVertexBuffer(const void *data, uint32_t sizeBytes,
                                            const RHIVertexLayout &layout);
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
// Debug Draw (Should not really be part of RHI, belongs to renderer)
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
void RHIDebugDrawTriangle(const Vector3 c1, const Vector3 c2, const Vector3 c3);
void RHIDebugDrawSetColor(uint32_t abgr);
void RHIDebugDrawLine(float fromX, float fromY, float fromZ,
                      float toX,   float toY,   float toZ);

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
