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

        win = std::make_unique<Window>(screenWidth, screenHeight);

        SetTargetFPS(60);

        while (!win->ShouldClose())
        {
            win->UpdateWindow();

            win->ClearWindow(255, 255, 255);



            win->Close();
        }

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

}