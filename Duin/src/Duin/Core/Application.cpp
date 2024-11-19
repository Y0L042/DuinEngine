#include "dnpch.h"
#include "Application.h"

#define RAYMATH_IMPLEMENTATION
#define RCAMERA_IMPLEMENTATION


static int screenWidth = 1280;
static int screenHeight = 720;

Color backgroundColor = WHITE;
::Camera3D activeCamera3D;

namespace duin
{
    static std::vector<std::function<void(double)>> postUpdateCallbacks;
    static std::vector<std::function<void(double)>> postPhysicsUpdateCallbacks;
    static std::vector<std::function<void(void)>> postDrawCallbacks;
    static std::vector<std::function<void(void)>> postDrawUICallbacks;

    void SetActiveCamera3D(::Camera3D camera3d)
    {
        activeCamera3D = camera3d;
    }

    void SetBackgroundColor(::Color color)
    {
        backgroundColor = color;
    }

	void QueuePostUpdateCallback(std::function<void(double)> f)
    {
        postUpdateCallbacks.push_back(f);
    }

	void QueuePostPhysicsUpdateCallback(std::function<void(double)> f)
    {
        postPhysicsUpdateCallbacks.push_back(f);
    }

	void QueuePostDrawCallback(std::function<void()> f)
    {
        postDrawCallbacks.push_back(f);
    }

	void QueuePostDrawUICallback(std::function<void()> f)
    {
        postDrawUICallbacks.push_back(f);
    }



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


    void Application::Run()
    {
        EngineInitialize();
        Initialize();

        ::SetTargetFPS(TARGET_RENDER_FRAMERATE);
        ::InitWindow(screenWidth, screenHeight, windowName.c_str());
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
            EnginePostUpdate(::GetFrameTime());

            if (::GetFrameTime() > 0.0f) {
                EnginePhysicsUpdate(::GetFrameTime()); // TODO
                PhysicsUpdate(::GetFrameTime()); // TODO
                EnginePostPhysicsUpdate(::GetFrameTime());
            }

            ::ClearBackground(::Color{ 
                backgroundColor.r, 
                backgroundColor.g, 
                backgroundColor.b, 
                backgroundColor.a });
            

                if (1) {
                    BeginMode3D(activeCamera3D);
                        EngineDraw();
                        Draw();
                        EnginePostDraw();
                    EndMode3D();
                } else {
                    EngineDraw();
                    Draw();
                    EnginePostDraw();
                }
                
                EngineDrawUI();
                DrawUI();
                EnginePostDrawUI();

            ::rlImGuiEnd();
            ::EndDrawing();

            EnginePostFrame();
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

    void Application::EngineUpdate(double delta)
    {
    }

    void Application::Update(double delta)
    {
    }

    void Application::EnginePostUpdate(double delta)
    {
        for (auto callback : postUpdateCallbacks) {
            callback(delta);
        }
    }

    void Application::EnginePhysicsUpdate(double delta)
    {
    }

    void Application::PhysicsUpdate(double delta)
    {
    }

    void Application::EnginePostPhysicsUpdate(double delta)
    {
        for (auto callback : postPhysicsUpdateCallbacks) {
            callback(delta);
        }
    }

    void Application::EngineDraw()
    {
    }

    void Application::Draw()
    {
    }

    void Application::EnginePostDraw()
    {
        for (auto callback : postDrawCallbacks) {
            callback();
        }
    }

    void Application::EngineDrawUI()
    {
    }

    void Application::DrawUI()
    {
    }

    void Application::EnginePostDrawUI()
    {
        for (auto callback : postDrawUICallbacks) {
            callback();
        }
    }

    void Application::EnginePostFrame()
    {
        postUpdateCallbacks.clear();
        postPhysicsUpdateCallbacks.clear();
        postDrawCallbacks.clear();
        postDrawUICallbacks.clear();
    }

    void Application::EngineExit()
    {
    }

    void Application::Exit()
    {
    }

}
