#pragma once

#include "RenderShape.h"
#include <cstdint>
#include <vector>
#include <unordered_map>

#include "Duin/Core/Utils/UUID.h"

namespace duin {
    struct ShaderProgram {
        UUID uuid;
        bgfx::ShaderHandle vsh;
        bgfx::ShaderHandle fsh;
        bgfx::ProgramHandle program;
        uint8_t isValid = 0;

        ShaderProgram() {}
        ShaderProgram(bgfx::ShaderHandle vsh, bgfx::ShaderHandle fsh, bgfx::ProgramHandle program)
            : vsh(vsh), fsh(fsh), program(program), isValid(1) 
        {}
    };

    class Renderer
    {
        public:
            struct BgfxShapeHandle {
                bgfx::VertexBufferHandle vbh;
                bgfx::IndexBufferHandle ibh;
                UUID shaderProgramUUID;
            };

            static Renderer& Get();

            Renderer();
            ~Renderer();

            void Init();
            void QueueShapeRender(BoxRenderShape& shape);
            void EmptyStack();
            void RenderPipeline();

        private:
            UUID defaultShaderProgramUUID;
            std::vector<BgfxShapeHandle> renderStack;
			bgfx::VertexLayout pcvDecl;
            std::unordered_map<UUID, ShaderProgram> shaderProgramMap;
    };
}
