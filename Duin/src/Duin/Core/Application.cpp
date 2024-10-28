#include "application.h"

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
   
        while(!::WindowShouldClose()) {
            ::BeginDrawing();
            ::rlImGuiBegin();

            // InputEvent e;
            EngineHandleInputs();
            HandleInputs();

            EngineUpdate(::GetFrameTime());
            Update(::GetFrameTime());

            if (::GetFrameTime() > 0.0f) {
                EnginePhysicsUpdate(::GetFrameTime()); // TODO
                PhysicsUpdate(::GetFrameTime()); // TODO
            }

            ::ClearBackground(::Color{ 
                backgroundColor.r, 
                backgroundColor.g, 
                backgroundColor.b, 
                backgroundColor.a });
            

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

    void Application::EngineHandleInputs()
    {
    }

    void Application::HandleInputs()
    {
    }

    void Application::EngineUpdate(double rDelta)
    {
    }

    void Application::Update(double rDelta)
    {
    }

    void Application::EnginePhysicsUpdate(double pDelta)
    {
    }

    void Application::PhysicsUpdate(double pDelta)
    {
    }

    void Application::EngineDraw()
    {
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

}
