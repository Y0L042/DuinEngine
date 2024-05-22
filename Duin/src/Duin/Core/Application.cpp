#include "dnpch.h"

#include "Application.h"
#include "Duin/Object/Node/Node2D/Node2D.h"

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

        InitWindow(screenWidth, screenHeight, "Test");
        SetTargetFPS(60);
        rlImGuiSetup(true);
        
        EngineInitialize();
        EngineReady();
        
        Initialize();
        Ready();
   
        while(!WindowShouldClose())
        {
            BeginDrawing();
            rlImGuiBegin();

            // TODO Generate event, then pass to handleinputs

            InputEvent e;
            EngineHandleInputs(e);
            HandleInputs(e);

            EngineProcess(win->GetFrametime());
            Process(win->GetFrametime());

            EnginePhysicsProcess(win->GetFrametime()); // TODO
            PhysicsProcess(win->GetFrametime()); // TODO

            ClearBackground(RAYWHITE);
            

            DrawText("Congrats! You created your first window!", 190, 200, 20, LIGHTGRAY);

            EngineDraw();
            Draw();

            rlImGuiEnd();
            EndDrawing();
        }

        EngineExit();
        Exit();

        rlImGuiShutdown();
        CloseWindow();
    }

    void Application::EngineInitialize()
    {
        ObjectManager::SetRootNode(std::make_shared<Node2D>());
    }

    void Application::Initialize()
    {
    }

    void Application::EngineReady()
    {
        ObjectManager::CallReady();
    }

    void Application::Ready()
    {
    }

    void Application::EngineHandleInputs(InputEvent e)
    {
        ObjectManager::CallHandleInput(e);
    }

    void Application::HandleInputs(InputEvent e)
    {
    }

    void Application::EngineProcess(double rDelta)
    {
        ObjectManager::CallUpdate(rDelta);
    }

    void Application::Process(double rDelta)
    {
    }

    void Application::EnginePhysicsProcess(double pDelta)
    {
        ObjectManager::CallPhysicsUpdate(pDelta);
    }

    void Application::PhysicsProcess(double pDelta)
    {
    }

    void Application::EngineDraw()
    {
        ObjectManager::CallDraw();
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