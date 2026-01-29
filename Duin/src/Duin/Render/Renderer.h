/**
 * @file Renderer.h
 * @brief Core rendering system and utilities.
 * @ingroup Render_Core
 *
 * Provides the main rendering interface using BGFX. Handles 3D rendering,
 * render-to-texture, debug drawing, and primitive shapes.
 */

#pragma once

#include "RenderGeometry.h"
#include "Camera.h"
#include "RenderShape.h"

#include "Duin/Core/Utils/UUID.h"
#include "Color.h"
#include <debugdraw/debugdraw.h>

#include <cstdint>
#include <vector>
#include <unordered_map>
#include <cmath>

namespace duin
{

struct RenderTexture;
struct RenderState;
struct ShaderProgram;
struct DebugDrawState;

/** @brief Initializes the BGFX rendering system. */
void InitRenderer();
/** @brief Shuts down the rendering system. */
void CleanRenderer();
/** @brief Returns true if rendering context is available. */
bool IsRenderContextAvailable();
/** @brief Sets rendering context availability flag. */
void SetRenderContextAvailable(bool available);

/**
 * @struct DebugDrawState
 * @brief State for debug drawing operations.
 * @ingroup Render_Core
 */
struct DebugDrawState
{
    bool isActive = false;
    DebugDrawEncoder encoder;
    bgfx::Encoder *bgfxEncoder = nullptr;
};

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
    DebugDrawState debugDrawState;
    Matrix viewMatrix;
    Matrix projectionMatrix;
    UUID stateUUID;
    size_t viewID = 0;
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
    uint16_t width, height;              ///< Texture dimensions.
    UUID textureUUID;                    ///< Unique identifier.
    bgfx::ViewId viewID = 0;             ///< BGFX view ID.
    bgfx::TextureHandle texture;         ///< BGFX texture handle.
    bgfx::FrameBufferHandle frameBuffer; ///< BGFX framebuffer handle.

    RenderTexture() = default;

    /**
     * @brief Creates a render texture.
     * @param width_ Width in pixels.
     * @param height_ Height in pixels.
     * @param textureUUID_ Unique identifier.
     */
    RenderTexture(int width_, int height_, UUID textureUUID_)
        : width(static_cast<uint16_t>(width_)), height(static_cast<uint16_t>(height_)), textureUUID(textureUUID_),
          texture(BGFX_INVALID_HANDLE), frameBuffer(BGFX_INVALID_HANDLE)
    {
        // Create uninitialized texture
        texture = bgfx::createTexture2D(
            static_cast<uint16_t>(std::max(width, (uint16_t)1)), static_cast<uint16_t>(std::max(height, (uint16_t)1)),
            false, // mipmaps
            1,     // num layers
            bgfx::TextureFormat::RGBA8,
            BGFX_TEXTURE_RT | BGFX_SAMPLER_MIN_POINT | BGFX_SAMPLER_MAG_POINT | BGFX_SAMPLER_MIP_POINT);
        frameBuffer = bgfx::createFrameBuffer(1, &texture);
    }

    /** @brief Returns true if texture handles are valid and dimensions > 0. */
    bool IsValid()
    {
        bool valid = bgfx::isValid(texture) && bgfx::isValid(frameBuffer);
        valid = valid && ((width > 0) && (height > 0));

        return valid;
    }

    /** @brief Releases BGFX resources. */
    void Destroy()
    {
        if (!IsRenderContextAvailable())
        {
            return;
        }

        textureUUID = UUID::INVALID;
        width = 0;
        height = 0;

        bgfx::destroy(texture);
        texture = BGFX_INVALID_HANDLE;

        bgfx::destroy(frameBuffer);
        frameBuffer = BGFX_INVALID_HANDLE;
    }
};

/**
 * @struct ShaderProgram
 * @brief Compiled shader program.
 * @ingroup Render_Shader
 */
struct ShaderProgram
{
    UUID uuid;
    bgfx::ShaderHandle vsh;      ///< Vertex shader handle.
    bgfx::ShaderHandle fsh;      ///< Fragment shader handle.
    bgfx::ProgramHandle program; ///< Combined program handle.
    uint8_t isValid = 0;

    ShaderProgram() : vsh(BGFX_INVALID_HANDLE), fsh(BGFX_INVALID_HANDLE), program(BGFX_INVALID_HANDLE), isValid(0)
    {
    }
    ShaderProgram(bgfx::ShaderHandle vsh, bgfx::ShaderHandle fsh, bgfx::ProgramHandle program)
        : vsh(vsh), fsh(fsh), program(program), isValid(1)
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
/** @} */

/**
 * @name Render Queue
 * @{
 */
void QueueRender(RenderGeometryType::Type type);
void QueueRender(RenderGeometryType::Type type, Vector3 position, Quaternion rotation, Vector3 size);
void ExecuteRenderPipeline();
void EmptyRenderStack();
/** @} */

/** @brief Clears the background to a solid color. */
void ClearBackground(Color color = Color(0x443355FF), int win = 0);

/**
 * @name Draw Functions
 * Immediate-mode drawing of primitives.
 * @{
 */
void DrawBox(Vector3 position = Vector3(), Quaternion rotation = QuaternionIdentity(),
             Vector3 size = Vector3(1.0f, 1.0f, 1.0f));
void DrawSphere(Vector3 position = Vector3(), Quaternion rotation = QuaternionIdentity(),
                Vector3 size = Vector3(1.0f, 1.0f, 1.0f));
void DrawSquare(Vector3 position = Vector3(), Quaternion rotation = QuaternionIdentity(),
                Vector3 size = Vector3(1.0f, 1.0f, 1.0f));
void DrawGrid(float size = 50);
void DrawPlane(Vector3 size);
void DrawDebugSphere(Vector3 position, float radius);
/** @} */

/** @brief Draws a RenderTexture as an ImGui image. Returns texture ID. */
unsigned int DrawIMGUITexture(RenderTexture &texture, Vector2 targetSize);
/** @brief Destroys a render texture and frees its resources. */
void DestroyRenderTexture(RenderTexture &texture);

/**
 * @name Test Accessors
 * Functions for testing render state without GPU context.
 * @{
 */
/** @brief Returns a copy of the current render state (for testing). */
RenderState GetCurrentRenderState();
/** @brief Returns the current render state stack depth (for testing). */
size_t GetRenderStateStackDepth();
/** @brief Resets render state to defaults without GPU calls (for testing). */
void ResetRenderStateForTesting();
/** @brief Simulates BeginTextureMode state changes without GPU calls (for testing). */
void BeginTextureModeForTesting(RenderTexture &target);
/** @brief Simulates EndTextureMode state changes without GPU calls (for testing). */
void EndTextureModeForTesting();
/** @} */

} // namespace duin
