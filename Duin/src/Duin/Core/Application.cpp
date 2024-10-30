#include "dnpch.h"
#include "application.h"

#define RAYMATH_IMPLEMENTATION
#define RCAMERA_IMPLEMENTATION

static int screenWidth = 1280;
static int screenHeight = 720;

::Camera2D *active_camera2d;
::Camera3D *active_camera3d;

namespace Duin
{
    Application::Application()
    {
    }

    Application::~Application()
    {
    }

    void Application::SetWindowStartupSize(int width, int height)
    {
        screenWidth = width;
        screenHeight = height;
    }

    void Application::SetBackgroundColor(::Color color)
    {
        backgroundColor = color;
    }

    void Application::SetWindowName(const char* string)
    {
        windowName = std::string(string);
    }

    void Application::SetActiveCamera2D(::Camera2D *camera2d)
    {
        active_camera2d = camera2d;
    }

    void Application::SetActiveCamera3D(::Camera3D *camera3d)
    {
        active_camera3d = camera3d;
    }

    void Application::Run()
    {
        EngineInitialize();
        Initialize();
        

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
            

                if (active_camera2d != NULL) {
                    BeginMode2D(*active_camera2d);
                        EngineDraw();
                        Draw();
                    EndMode2D();
                } else if (active_camera3d != NULL) {
                    BeginMode3D(*active_camera3d);
                        EngineDraw();
                        Draw();
                    EndMode3D();
                } else {
                    EngineDraw();
                    Draw();
                }
                
                EngineDrawUI();
                DrawUI();

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

    void Application::EngineDrawUI()
    {
    }

    void Application::DrawUI()
    {
    }

    void Application::EngineExit()
    {
    }

    void Application::Exit()
    {
    }

}
