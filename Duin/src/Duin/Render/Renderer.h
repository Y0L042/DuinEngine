#pragma once

#include "RenderGeometry.h"
#include "Camera.h"
#include "RenderShape.h"

#include "Duin/Core/Utils/UUID.h"
#include <debugdraw/debugdraw.h>

#include <cstdint>
#include <vector>
#include <unordered_map>


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
        UUID viewID;
    };

    struct RenderTexture {
        int width, height;
        UUID viewID;
        bgfx::TextureHandle texture;
        bgfx::FrameBufferHandle frameBuffer;


        RenderTexture() = default;
        RenderTexture(float width, float height, UUID viewID)
			: width(width), height(height), viewID(viewID), texture(BGFX_INVALID_HANDLE), frameBuffer(BGFX_INVALID_HANDLE)
        {
            bgfx::createTexture2D(
                width, 
                height,
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

        void Destroy()
        {
			bgfx::destroy(frameBuffer);
            bgfx::destroy(texture);
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
    void DestroyRenderTexture(RenderTexture& texture);


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


}
