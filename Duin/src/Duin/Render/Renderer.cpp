#include "dnpch.h"
#include "Renderer.h"

#include "Shader.h"
#include <bx/math.h>
#include <debugdraw/debugdraw.h>


#include "Duin/Core/Debug/DNLog.h"
#include "Duin/Core/Application.h"

namespace duin {
    DebugDrawEncoder dde;
	bgfx::Encoder *encoder = nullptr;

    void DrawBox(Vector3 position, Quaternion rotation, Vector3 size)
    {
        Renderer::Get().QueueRender(RenderGeometryType::BOX, position, rotation, size);
    }

    void DrawSquare(Vector3 position, 
                 Quaternion rotation, 
                 Vector3 size)
    {
        Renderer::Get().QueueRender(RenderGeometryType::PLANE, position, rotation, size);
    }

    void DrawGrid()
    {
        Renderer::Get().StartDebugDraw();
        dde.drawGrid(Axis::Y, {0.0f, 0.0f, 0.0f}, 20, 1.0f);
		Renderer::Get().EndDebugDraw();
    }

    void DrawPlane(Vector3 size)
    {
        Vector3 position = Vector3Zero();
        // Quaternion rotation( 0.0f, 0.0f, 0.3007058f, 0.953717f );// = QuaternionIdentity();
        Quaternion rotation = QuaternionFromEuler(0.4f, 0.4f, 0.4f);
        Renderer::Get().QueueRender(RenderGeometryType::PLANE, position, rotation, size);
    }

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
        
        ddInit();
        pcvDecl.begin()
            .add(bgfx::Attrib::Position, 3, bgfx::AttribType::Float)
            .add(bgfx::Attrib::Color0, 4, bgfx::AttribType::Uint8, true)
        .end();

        bgfx::ShaderHandle vsh = LoadShader("C:/Projects/CPP_Projects/Duin/Duin/src/Duin/Resources/shaders/dx11/vs_cubes.bin");
        bgfx::ShaderHandle fsh = LoadShader("C:/Projects/CPP_Projects/Duin/Duin/src/Duin/Resources/shaders/dx11/fs_cubes.bin");
        bgfx::ProgramHandle program = bgfx::createProgram(vsh, fsh, true); 
        ShaderProgram shaderProgram(vsh, fsh, program);
        shaderProgramMap[shaderProgram.uuid] = shaderProgram;
        DEFAULT_SHADERPROGRAM_UUID = shaderProgram.uuid;

        CreateGeometryBuffers();

        DN_CORE_INFO("Renderer initialised.");
    }

    void Renderer::QueueRender(RenderGeometryType::Type type)
    {
        bgfx::VertexBufferHandle vbh = BGFX_INVALID_HANDLE;
        bgfx::IndexBufferHandle ibh = BGFX_INVALID_HANDLE;
        bgfx::ProgramHandle program = shaderProgramMap[DEFAULT_SHADERPROGRAM_UUID].program;

        Renderer::BGFXBufferHandle buffers = Renderer::Get().GetGeometryBufferHandle(type);
        vbh = buffers.vbh;
        ibh = buffers.ibh;

        bgfx::setVertexBuffer(0, vbh);
        bgfx::setIndexBuffer(ibh);
        bgfx::submit(0, program);
    }

    void Renderer::QueueRender(RenderGeometryType::Type type, Vector3 position, Quaternion rotation, Vector3 size)
    {
        Vector3 eulerRotation = QuaternionToEuler(rotation);

        bgfx::VertexBufferHandle vbh = BGFX_INVALID_HANDLE;
        bgfx::IndexBufferHandle ibh = BGFX_INVALID_HANDLE;
        bgfx::ProgramHandle program = shaderProgramMap[DEFAULT_SHADERPROGRAM_UUID].program;

        Renderer::BGFXBufferHandle buffers = Renderer::Get().GetGeometryBufferHandle(type);
        vbh = buffers.vbh;
        ibh = buffers.ibh;
        if (!(bgfx::isValid(vbh) && bgfx::isValid(ibh))) {
            DN_CORE_WARN("Invalid geometry buffers submitted!");
            return;
        }


        float mtx[16];
        bx::mtxSRT(mtx, 
                   size.x, size.y, size.z,
                   eulerRotation.x, eulerRotation.y, eulerRotation.z, 
                   position.x, position.y, position.z);
        bgfx::setTransform(mtx);
        bgfx::setVertexBuffer(0, vbh);
        bgfx::setIndexBuffer(ibh);
        bgfx::submit(0, program);
    }

    void Renderer::EmptyStack() {}

    void Renderer::RenderPipeline()
    {
        int screenWidth = GetWindowWidth();
		int screenHeight = GetWindowHeight();

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

    }

    void Renderer::StartDebugDraw()
    {
        encoder = bgfx::begin();
        dde.begin(0, true, encoder);
    }

    void Renderer::EndDebugDraw()
    {
        dde.end();
		bgfx::end(encoder);
    }

    Renderer::BGFXBufferHandle Renderer::GetGeometryBufferHandle(RenderGeometryType::Type type)
    {
        switch (type) {
            case RenderGeometryType::BOX:
                return bgfxBufferList[RenderGeometryType::BOX];
                break;
            case RenderGeometryType::PLANE:
                return bgfxBufferList[RenderGeometryType::PLANE];
                break;
            default:
                break;
        }
    }

    void Renderer::CreateGeometryBuffers()
    {
        bgfxBufferList.resize(RenderGeometryType::Count);

        bgfx::VertexBufferHandle vbh = BGFX_INVALID_HANDLE;
        bgfx::IndexBufferHandle ibh = BGFX_INVALID_HANDLE;

        /* Create BOX Buffers */
        vbh = BGFX_INVALID_HANDLE;
        ibh = BGFX_INVALID_HANDLE;
        vbh = bgfx::createVertexBuffer(bgfx::makeRef(BoxRenderGeometry::GetIdentityVertices(),
                                                     BoxRenderGeometry::VertSize() * sizeof(PosColorVertex)), pcvDecl);
        ibh = bgfx::createIndexBuffer(bgfx::makeRef(BoxRenderGeometry::GetIdentityTriList(),
                                                    BoxRenderGeometry::TriSize() * sizeof(uint16_t)));
        bgfxBufferList[RenderGeometryType::BOX] = Renderer::BGFXBufferHandle(vbh, ibh);

		/* Create PLANE Buffers */
        vbh = BGFX_INVALID_HANDLE;
        ibh = BGFX_INVALID_HANDLE;
        vbh = bgfx::createVertexBuffer(bgfx::makeRef(PlaneRenderGeometry::GetIdentityVertices(),
            PlaneRenderGeometry::VertSize() * sizeof(PosColorVertex)), pcvDecl);
        ibh = bgfx::createIndexBuffer(bgfx::makeRef(PlaneRenderGeometry::GetIdentityTriList(),
            PlaneRenderGeometry::TriSize() * sizeof(uint16_t)));
        bgfxBufferList[RenderGeometryType::PLANE] = Renderer::BGFXBufferHandle(vbh, ibh);
    }
}
