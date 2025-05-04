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


namespace duin {
    struct RenderTexture;
    struct RenderState;
	struct ShaderProgram;
    struct DebugDrawState;

    struct DebugDrawState {
        bool isActive = false;
        DebugDrawEncoder encoder;
        bgfx::Encoder* bgfxEncoder = nullptr;
    };

    struct RenderState {
        RenderTexture* target = nullptr;
        Camera* camera = nullptr;
		bool in3DMode = false;
		bool inTextureMode = false;
        DebugDrawState debugDrawState;
		Matrix viewMatrix;
		Matrix projectionMatrix;
        UUID stateUUID;
        size_t viewID = 0;
    };

    struct RenderTexture {
        int width, height;
        UUID textureUUID;
        size_t viewID = 0;
        bgfx::TextureHandle texture;
        bgfx::FrameBufferHandle frameBuffer;


        RenderTexture() = default;
        RenderTexture(float width, float height, UUID viewID)
			: width(width), height(height), viewID(viewID), texture(BGFX_INVALID_HANDLE), frameBuffer(BGFX_INVALID_HANDLE)
        {
            texture = bgfx::createTexture2D(
                std::max(width, 0.1f),
                std::max(height, 0.1f),
                false, // mipmaps
                1, // num layers
                bgfx::TextureFormat::RGBA8,
                BGFX_TEXTURE_RT
                | BGFX_SAMPLER_MIN_POINT
                | BGFX_SAMPLER_MAG_POINT
                | BGFX_SAMPLER_MIP_POINT
            );
            frameBuffer = bgfx::createFrameBuffer(1, &texture);
        }

        bool IsValid()
        {
            bool valid = bgfx::isValid(texture) && bgfx::isValid(frameBuffer);
            valid = valid && ((width > 0) && (height > 0));
            return valid;
        }

        void Destroy()
        {
            if (bgfx::isValid(frameBuffer) && frameBuffer.idx != bgfx::kInvalidHandle)
            {
                bgfx::destroy(frameBuffer);
                frameBuffer = BGFX_INVALID_HANDLE;
            }
                
            if (bgfx::isValid(texture) && texture.idx != bgfx::kInvalidHandle)
            {
                bgfx::destroy(texture);
                texture = BGFX_INVALID_HANDLE;
            }
            bgfx::touch(0);
        }
    };

    struct ShaderProgram {
        UUID uuid;
        bgfx::ShaderHandle vsh;
        bgfx::ShaderHandle fsh;
        bgfx::ProgramHandle program;
        uint8_t isValid = 0;

        ShaderProgram() : vsh(BGFX_INVALID_HANDLE), fsh(BGFX_INVALID_HANDLE), program(BGFX_INVALID_HANDLE), isValid(0) {}
        ShaderProgram(bgfx::ShaderHandle vsh, bgfx::ShaderHandle fsh, bgfx::ProgramHandle program)
            : vsh(vsh), fsh(fsh), program(program), isValid(1) 
        {}
    };

    void InitRenderer();

    void BeginDraw3D(Camera& camera);
    void EndDraw3D();
    void BeginTextureMode(RenderTexture& target);
    void EndTextureMode();
    void BeginDebugDraw();
    void EndDebugDraw();

    void QueueRender(RenderGeometryType::Type type);
    void QueueRender(RenderGeometryType::Type type, Vector3 position, Quaternion rotation, Vector3 size);
    // TODO add function to allow for custom geometry
    // void QueueRender(RenderGeometryType::Type type, Vector3 position, Quaternion rotation, Vector3 size);
    void ExecuteRenderPipeline();
    void EmptyRenderStack();
    void ClearBackground(Color color = Color(0x443355FF), int win = 0);


    void DrawBox(Vector3 position = Vector3(), 
                 Quaternion rotation = QuaternionIdentity(), 
                 Vector3 size = Vector3(1.0f, 1.0f, 1.0f));
    void DrawSphere(Vector3 position = Vector3(), 
                 Quaternion rotation = QuaternionIdentity(), 
                 Vector3 size = Vector3(1.0f, 1.0f, 1.0f));
    void DrawSquare(Vector3 position = Vector3(), 
                 Quaternion rotation = QuaternionIdentity(), 
                 Vector3 size = Vector3(1.0f, 1.0f, 1.0f));
    void DrawGrid();
    void DrawPlane(Vector3 size);
    void DrawDebugSphere(Vector3 position, float radius);


    unsigned int DrawIMGUITexture(RenderTexture& texture, Vector2 targetSize);
    void DestroyRenderTexture(RenderTexture& texture);

}
