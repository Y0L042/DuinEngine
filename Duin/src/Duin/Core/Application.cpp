#include "dnpch.h"

#include "Application.h"
#include "Duin/Object/Node/CanvasItem/Node2D/Node2D.h"

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
        
        EngineInitialize();
        EngineReady();
        
        Initialize();
        Ready();

        while (!win->ShouldClose())
        {
            win->UpdateWindow();
            win->ClearWindow(255, 255, 255);

            // TODO Generate event, then pass to handleinputs

            //EngineHandleInputs(e);
            //HandleInputs(e);

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

    void Application::EngineInitialize()
    {
        auto rootNode = std::make_shared<Node2D>(); // 2D for now
        ObjectManager::SetRootNode(rootNode); 
    }

    void Application::Initialize()
    {
    }

    void Application::EngineReady()
    {
    }

    void Application::Ready()
    {
        ObjectManager::CallReady();
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