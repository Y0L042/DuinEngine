#pragma once

#include "RenderShape.h"
#include <cstdint>
#include <vector>
#include <unordered_map>

#include "Duin/Core/Utils/UUID.h"
#include "RenderGeometry.h"

namespace duin {
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

    struct PosColorVertex;
    class Renderer
    {
        public:
            struct BGFXBufferHandle {
                UUID uuid;
                bgfx::VertexBufferHandle vbh = BGFX_INVALID_HANDLE;
                bgfx::IndexBufferHandle ibh = BGFX_INVALID_HANDLE;

				BGFXBufferHandle() : vbh(BGFX_INVALID_HANDLE), ibh(BGFX_INVALID_HANDLE) {}
                BGFXBufferHandle(bgfx::VertexBufferHandle vbh, bgfx::IndexBufferHandle ibh)
                    : vbh(vbh), ibh(ibh) {}
            };


            static Renderer& Get();

            Renderer();
            ~Renderer();

            void Init();
            void QueueRender(RenderGeometryType::Type type);
            void QueueRender(RenderGeometryType::Type type, Vector3 position, Quaternion rotation, Vector3 size);
            // TODO add function to allow for custom geometry
            // void QueueRender(RenderGeometryType::Type type, Vector3 position, Quaternion rotation, Vector3 size);
            void EmptyStack();
            void RenderPipeline();
            void StartDebugDraw();
            void EndDebugDraw();

            BGFXBufferHandle GetGeometryBufferHandle(RenderGeometryType::Type type);

        private:
            UUID DEFAULT_SHADERPROGRAM_UUID;
			bgfx::VertexLayout pcvDecl;
            std::vector<BGFXBufferHandle> bgfxBufferList;
            std::unordered_map<UUID, ShaderProgram> shaderProgramMap;

            void CreateGeometryBuffers();
    };
}
