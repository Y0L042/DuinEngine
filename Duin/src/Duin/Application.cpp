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

        raylib::Window window(screenWidth, screenHeight, "raylib-cpp - basic window");
        //raylib::Texture logo("raylib_logo.png");

        SetTargetFPS(60);

        while (!window.ShouldClose())
        {
            BeginDrawing();

            window.ClearBackground(RAYWHITE);

            DrawText("Congrats! You created your first window!", 190, 200, 20, LIGHTGRAY);

            // Object methods.
            //logo.Draw(
            //    screenWidth / 2 - logo.GetWidth() / 2,
            //    screenHeight / 2 - logo.GetHeight() / 2);

            EndDrawing();
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