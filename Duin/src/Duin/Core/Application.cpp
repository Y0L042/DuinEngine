#include "dnpch.h"
#include "Application.h"

#define RAYMATH_IMPLEMENTATION
#define RCAMERA_IMPLEMENTATION

static size_t physicsFrameCount = 0;
static size_t renderFrameCount = 0;

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
    static std::vector<std::function<void(void)>> preFrameCallbacks;
    static std::vector<std::function<void(void)>> postFrameCallbacks;

    void SetActiveCamera3D(::Camera3D camera3d)
    {
        activeCamera3D = camera3d;
    }

    void SetBackgroundColor(::Color color)
    {
        backgroundColor = color;
    }

    float GetPhysicsFPS()
    {
        float fps = ::GetFPS();
        return fps;
    }

    float GetPhysicsFrameTime()
    {
        float frametime = ::GetFrameTime();
        return frametime;
    }

    float GetRenderFPS()
    {
        float fps = ::GetFPS();
        return fps;
    }

    float GetRenderFrameTime()
    {
        float frametime = ::GetFrameTime();
        return frametime;
    }

    size_t GetPhysicsFrameCount()
    {
        return physicsFrameCount;
    }

    size_t GetRenderFrameCount()
    {
        return renderFrameCount;
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

    void QueuePreFrameCallback(std::function<void()> f)
    {
        preFrameCallbacks.push_back(f);
    }

    void QueuePostFrameCallback(std::function<void()> f)
    {
        postFrameCallbacks.push_back(f);
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

            EnginePreFrame();

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

    void Application::EnginePreFrame()
    {
        for (auto& callback : preFrameCallbacks) {
            callback();
        }
    }

    void Application::EngineHandleInputs()
    {
    }

    void Application::HandleInputs()
    {
    }

    void Application::EngineUpdate(double delta)
    {
        ++renderFrameCount;
    }

    void Application::Update(double delta)
    {
    }

    void Application::EnginePostUpdate(double delta)
    {
        for (auto& callback : postUpdateCallbacks) {
            callback(delta);
        }
    }

    void Application::EnginePhysicsUpdate(double delta)
    {
        ++physicsFrameCount;
    }

    void Application::PhysicsUpdate(double delta)
    {
    }

    void Application::EnginePostPhysicsUpdate(double delta)
    {
        for (auto& callback : postPhysicsUpdateCallbacks) {
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
        for (auto& callback : postDrawCallbacks) {
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
        for (auto& callback : postDrawUICallbacks) {
            callback();
        }
    }

    void Application::EnginePostFrame()
    {
        for (auto& callback : postFrameCallbacks) {
            callback();
        }
    }

    void Application::EngineExit()
    {
    }

    void Application::Exit()
    {
    }

}
