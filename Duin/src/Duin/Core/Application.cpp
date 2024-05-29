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

    void Application::Run()
    {
        int screenWidth = 800;
        int screenHeight = 450;

        ::InitWindow(screenWidth, screenHeight, "Test");
        ::SetTargetFPS(144);
        ::rlImGuiSetup(true);
        
        EngineInitialize();
        EngineReady();
        
        Initialize();
        Ready();
   
        while(!::WindowShouldClose())
        {
            ::BeginDrawing();
            ::rlImGuiBegin();

            // TODO Generate event, then pass to handleinputs

            InputEvent e;
            EngineHandleInputs(e);
            HandleInputs(e);

            EngineProcess(::GetFrameTime());
            Process(::GetFrameTime());

            EnginePhysicsProcess(::GetFrameTime()); // TODO
            PhysicsProcess(::GetFrameTime()); // TODO

            ::ClearBackground(RAYWHITE);
            

            ::DrawText("Congrats! You created your first window!", 190, 200, 20, LIGHTGRAY);

            EngineDraw();
            Draw();

            ::rlImGuiEnd();
            ::EndDrawing();
        }

        EngineExit();
        Exit();

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
    }

    void Application::Ready()
    {
    }

    void Application::EngineHandleInputs(InputEvent e)
    {
    }

    void Application::HandleInputs(InputEvent e)
    {
    }

    void Application::EngineProcess(double rDelta)
    {
    }

    void Application::Process(double rDelta)
    {
    }

    void Application::EnginePhysicsProcess(double pDelta)
    {
    }

    void Application::PhysicsProcess(double pDelta)
    {
    }

    void Application::EngineDraw()
    {
        int fps = ::GetFPS();
        std::string fpsStr = std::to_string(fps);
        ::DrawText(fpsStr.c_str(), 10, 10, 25, GREEN);
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