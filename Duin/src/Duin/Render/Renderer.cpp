#include "dnpch.h"
#include "Renderer.h"

#include "Shader.h"
#include <bx/math.h>

#include "Duin/Core/Debug/DNLog.h"
#include "Duin/Core/Application.h"

namespace duin {
    Renderer& Renderer::Get()
    {
        static Renderer renderer;
        return renderer;
    }

    Renderer::Renderer() 
    {
        Init();
    }

    Renderer:: ~Renderer() {}


    void Renderer::Init()
    {
        pcvDecl.begin()
            .add(bgfx::Attrib::Position, 3, bgfx::AttribType::Float)
            .add(bgfx::Attrib::Color0, 4, bgfx::AttribType::Uint8, true)
        .end();

        bgfx::ShaderHandle vsh = LoadShader("C:/Projects/CPP_Projects/Duin/Duin/src/Duin/Resources/shaders/dx11/vs_cubes.bin");
        bgfx::ShaderHandle fsh = LoadShader("C:/Projects/CPP_Projects/Duin/Duin/src/Duin/Resources/shaders/dx11/fs_cubes.bin");
        bgfx::ProgramHandle program = bgfx::createProgram(vsh, fsh, true); 
        ShaderProgram shaderProgram(vsh, fsh, program);
        shaderProgramMap[shaderProgram.uuid] = shaderProgram;
        defaultShaderProgramUUID = shaderProgram.uuid;

        DN_CORE_INFO("Renderer initialised.");
    }

    void Renderer::QueueShapeRender(BoxRenderShape& shape)
    {
        BgfxShapeHandle handle_;
        renderStack.push_back(handle_);
        BgfxShapeHandle& handle = renderStack.back();

        switch (shape.GetType()) {
            case RenderShapeType::BOX:
                DN_CORE_INFO("BOX added to render queue.");
                handle.vbh = bgfx::createVertexBuffer(bgfx::makeRef(shape.vertices, sizeof(shape.vertices)), pcvDecl);
                handle.ibh = bgfx::createIndexBuffer(bgfx::makeRef(shape.triList, sizeof(shape.triList)));
                break;
            default: 
                DN_CORE_WARN("NONE added to render queue!");
                break;
        }

    }

    void Renderer::EmptyStack()
    {
        renderStack.clear();
        DN_CORE_INFO("Clear render stack.");
    }

    void Renderer::RenderPipeline()
    {
        int screenWidth = GetWindowWidth();
		int screenHeight = GetWindowHeight();

        DN_CORE_INFO("Executing render pipeline.");


        static int counter = 0;
        counter++;
        const bx::Vec3 at = {0.0f, 0.0f,  0.0f};
        const bx::Vec3 eye = {0.0f, 0.0f, -5.0f};
        float view[16];
        bx::mtxLookAt(view, eye, at);
        float proj[16];
        bx::mtxProj(proj, 60.0f, float(screenWidth) / float(screenHeight), 0.1f, 100.0f, bgfx::getCaps()->homogeneousDepth);
        bgfx::setViewTransform(0, view, proj);

        float mult = 1.0f;
        float mtx[16];
        bx::mtxRotateXY(mtx, counter * 0.01f * mult, counter * 0.01f * mult);
        bgfx::setTransform(mtx);        

		for (BgfxShapeHandle& handle : renderStack) {
            ShaderProgram& shaderProgram = shaderProgramMap[defaultShaderProgramUUID];
            if (shaderProgram.isValid) {
                DN_CORE_INFO("Shape submitted for rendering.");
                bgfx::setVertexBuffer(0, handle.vbh);
                bgfx::setIndexBuffer(handle.ibh);

                bgfx::submit(0, shaderProgram.program);

                mult++;
            } else {
                DN_CORE_WARN("Shape's ShaderProgram invalid, not submitted!");
            }
        }
    }
}
