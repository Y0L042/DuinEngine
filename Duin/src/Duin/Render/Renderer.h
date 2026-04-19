/**
 * @file Renderer.h
 * @brief Core rendering system and utilities.
 * @ingroup Render_Core
 *
 * Provides the main rendering interface. Handles 3D rendering,
 * render-to-texture, debug drawing, and primitive shapes.
 * All GPU operations are routed through the RHI layer.
 */

#pragma once

#include "RHI.h"
#include "RenderGeometry.h"
#include "Camera.h"
#include "RenderShape.h"

#include "Duin/Core/Utils/UUID.h"
#include "Color.h"

#include <cstdint>
#include <vector>
#include <unordered_map>
#include <cmath>

namespace duin
{

#define RENDER_3D_VIEWID 0

struct RenderTexture;
struct RenderState;
struct ShaderProgram;

/** @brief Initializes the rendering system. */
void InitRenderer();
/** @brief Shuts down the rendering system. */
void CleanRenderer();
/** @brief Destroys GPU resources and resets all static state. Safe to call before shutdown. */
void ResetRenderer();
/** @brief Returns true if rendering context is available. */
bool IsRenderContextAvailable();
/** @brief Sets rendering context availability flag. */
void SetRenderContextAvailable(bool available);

/**
 * @struct RenderState
 * @brief Current render configuration.
 * @ingroup Render_Core
 *
 * Tracks active render target, camera, matrices, and view ID.
 */
struct RenderState
{
    RenderTexture *target = nullptr; ///< Active render target (nullptr = backbuffer).
    Camera *camera = nullptr;        ///< Active camera.
    bool in3DMode = false;           ///< True between BeginDraw3D/EndDraw3D.
    bool inTextureMode = false;      ///< True between BeginTextureMode/EndTextureMode.
    Matrix viewMatrix;
    Matrix projectionMatrix;
    UUID stateUUID;
    RHIViewId viewID = RHI_VIEW_3D;
};

/**
 * @struct RenderTexture
 * @brief Offscreen render target.
 * @ingroup Render_Core
 *
 * A texture that can be rendered to. Useful for viewports, post-processing,
 * and ImGui texture display.
 */
struct RenderTexture
{
    uint16_t width = 0;
    uint16_t height = 0;
    UUID textureUUID;                          ///< Unique identifier.
    RHIViewId viewID = RHI_VIEW_RENDERTEXTURE; ///< View ID.
    RHITextureHandle texture;                  ///< Texture handle.
    RHIFrameBufferHandle frameBuffer;          ///< Framebuffer handle.

    RenderTexture() = default;

    /**
     * @brief Creates a render texture.
     * @param width_ Width in pixels.
     * @param height_ Height in pixels.
     * @param textureUUID_ Unique identifier.
     */
    RenderTexture(int width_, int height_, UUID textureUUID_);

    /** @brief Returns true if texture handles are valid and dimensions > 0. */
    bool IsValid();

    /** @brief Releases GPU resources. */
    void Destroy();
};

/**
 * @struct ShaderProgram
 * @brief Compiled shader program.
 * @ingroup Render_Shader
 */
struct ShaderProgram
{
    UUID uuid;
    RHIShaderHandle vsh;      ///< Vertex shader handle.
    RHIShaderHandle fsh;      ///< Fragment shader handle.
    RHIProgramHandle program; ///< Combined program handle.
    uint8_t isValid = 0;

    ShaderProgram() : isValid(0)
    {
    }
    ShaderProgram(RHIShaderHandle vsh_, RHIShaderHandle fsh_, RHIProgramHandle program_)
        : vsh(vsh_), fsh(fsh_), program(program_), isValid(1)
    {
    }
};

/**
 * @name Render Mode Functions
 * Functions to enter/exit rendering modes.
 * @{
 */

/** @brief Begins 3D rendering with the specified camera. */
void BeginDraw3D(Camera &camera);
/** @brief Ends 3D rendering mode. */
void EndDraw3D();
/** @brief Begins rendering to a texture. */
void BeginTextureMode(RenderTexture &target);
/** @brief Ends texture rendering mode. */
void EndTextureMode();
/** @brief Begins debug draw mode. */
void BeginDebugDraw();
/** @brief Ends debug draw mode. */
void EndDebugDraw();
/** @brief Begins a minimal render frame (encoder + debug draw). For testing use. */
void BeginEncoderFrame();
/** @brief Ends a minimal render frame. Submits frame. For testing use. */
void EndEncoderFrame();
/** @} */

/**
 * @name Render Queue
 * @{
 */
void QueueRender(const RenderGeometryType::Type type);
void QueueRender(
    const RenderGeometryType::Type type, const Vector3 position, const Quaternion rotation, const Vector3 size);
void ExecuteRenderPipeline();
void EmptyRenderStack();
/** @} */

/** @brief Clears the background to a solid color. */
void ClearBackground(Color color = Color(0x443355FF));

/**
 * @name Draw Functions
 * Immediate-mode drawing of primitives.
 * @{
 */
void DrawBox(
    const Vector3 position = Vector3(), const Quaternion rotation = QuaternionIdentity(),
    const Vector3 size = Vector3(1.0f, 1.0f, 1.0f));
void DrawSphere(
    const Vector3 position = Vector3(), const Quaternion rotation = QuaternionIdentity(),
    const Vector3 size = Vector3(1.0f, 1.0f, 1.0f));
void DrawSquare(
    const Vector3 position = Vector3(), const Quaternion rotation = QuaternionIdentity(),
    const Vector3 size = Vector3(1.0f, 1.0f, 1.0f));
void DrawPlane(const Vector3 size);

/**
 * @name Debug Draw Functions
 * @{
 */
void DrawDebugTriangle(const Vector3 c1, const Vector3 c2, const Vector3 c3);
void DrawDebugGrid(const float size = 50);
void DrawDebugSphere(const Vector3 position, const float radius);
void DrawDebugCapsule(const Vector3 from, const Vector3 to, const float radius);
void DrawDebugBox(const Vector3 min, const Vector3 max);
/** @} */

/** @brief Draws a RenderTexture as an ImGui image. Returns texture ID. */
unsigned int DrawIMGUITexture(RenderTexture &texture, Vector2 targetSize);
/** @brief Destroys a render texture and frees its resources. */
void DestroyRenderTexture(RenderTexture &texture);

RenderState GetGlobalRenderState();

} // namespace duin
