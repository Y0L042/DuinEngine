#include "dnpch.h"

#include "Application.h"

namespace Duin
{

    Application::Application()
    {
    }

    Application::~Application()
    {
    }

    void Application::SetBackgroundColor(Color color)
    {
        backgroundColor = color;
    }

    void Application::SetWindowName(const char* string)
    {
        windowName = std::string(string);
    }

    void Application::SetRenderMode(Mode renderMode)
    {
        this->renderMode = renderMode;
    }

    void Application::SetCurrentCamera(Camera* camera)
    {
        currentCamera = camera;
    }

    void Application::Run()
    {
        EngineInitialize();
        Initialize();
        
        int screenWidth = 1280;
        int screenHeight = 720;

        ::InitWindow(screenWidth, screenHeight, windowName.c_str());
        ::SetTargetFPS(144);
        ::rlImGuiSetup(true);
        
        EngineReady();
        Ready();
   
        while(!::WindowShouldClose())
        {
            ::BeginDrawing();
            ::rlImGuiBegin();

            if (renderMode == MODE2D)
            {
                //BeginMode2D();
            }
            else if (renderMode == MODE3D)
            {
                //BeginMode3D();
            }

            InputEvent e;
            EngineHandleInputs(e);
            HandleInputs(e);

            EngineUpdate(::GetFrameTime());
            Update(::GetFrameTime());

            if (::GetFrameTime() > 0.0f)
            {
                EnginePhysicsUpdate(::GetFrameTime()); // TODO
                PhysicsUpdate(::GetFrameTime()); // TODO
            }

            ::ClearBackground(::Color{ backgroundColor.r, backgroundColor.g, backgroundColor.b, backgroundColor.a });
            

            Draw();
            EngineDraw();

            ::rlImGuiEnd();
            ::EndDrawing();
        }

        Exit();
        EngineExit();

        if (renderMode == MODE2D)
        {
            ::EndMode2D();
        }
        else if (renderMode == MODE3D)
        {
            ::EndMode3D();
        }

        ::rlImGuiShutdown();
        ::CloseWindow();
    }

    void Application::EngineInitialize()
    {
    }

    void Application::Initialize()
    {
    }

    void Application::EngineReady()
    {
        sceneManager.CallReady();
    }

    void Application::Ready()
    {
    }

    void Application::EngineHandleInputs(InputEvent e)
    {
        sceneManager.CallHandleInput(e);
    }

    void Application::HandleInputs(InputEvent e)
    {
    }

    void Application::EngineUpdate(double rDelta)
    {
        sceneManager.CallUpdate(rDelta);
    }

    void Application::Update(double rDelta)
    {
    }

    void Application::EnginePhysicsUpdate(double pDelta)
    {
        sceneManager.CallPhysicsUpdate(pDelta);
    }

    void Application::PhysicsUpdate(double pDelta)
    {
    }

    void Application::EngineDraw()
    {
        sceneManager.CallDraw();
        DebugDraw::Draw();
    }

    void Application::Draw()
    {
    }

    void Application::EngineExit()
    {
    }

    void Application::Exit()
    {
    }

    SceneManager& Application::GetSceneManager()
    {
        return sceneManager;
    }

}