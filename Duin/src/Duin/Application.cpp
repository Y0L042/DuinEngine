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

        win = std::make_unique<AppWindow>(screenWidth, screenHeight);
        win->SetTargetAppFPS(60);
        
        EngineReady();
        Ready();

        while (!win->ShouldClose())
        {
            win->UpdateWindow();
            win->ClearWindow(255, 255, 255);

            EngineHandleEvents();
            HandleEvents();

            EngineProcess(win->GetFrametime());
            Process(win->GetFrametime());

            EnginePhysicsProcess(win->GetFrametime()); // TODO
            PhysicsProcess(win->GetFrametime()); // TODO

            EngineDraw();
            Draw();

            win->Close();
        }

        EngineExit();
        Exit();

        win->Shutdown();
        // UnloadTexture() and CloseWindow() are called automatically.
    }

    void Application::EngineReady()
    {
    }

    void Application::Ready()
    {
    }

    void Application::EngineHandleEvents()
    {
    }

    void Application::HandleEvents()
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