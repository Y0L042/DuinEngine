#include "dnpch.h"
#include "Application.h"

//#include <imgui_impl_glfw.h>
//#include <imgui_impl_opengl3.h>

#define RAYMATH_IMPLEMENTATION
#define RCAMERA_IMPLEMENTATION

static int debugIsGamePaused_ = 0;

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
    static std::vector<std::function<void(void)>> postDebugCallbacks;

    void DebugPauseGame()
    {
        debugIsGamePaused_ = 1;
    }

    void DebugResumeGame()
    {
        debugIsGamePaused_ = 0;
    }

    int DebugIsGamePaused()
    {
        return debugIsGamePaused_;
    }

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

	void QueuePostDebugCallback(std::function<void()> f)
    {
        postDebugCallbacks.push_back(f);
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
        // Custom timming variables
        double previousTime = GetTime();    // Previous time measure
        double currentTime = 0.0;           // Current time measure
        double updateDrawTime = 0.0;        // Update + Draw time
        double waitTime = 0.0;              // Wait time (if target fps required)
        float deltaTime = 0.0f;             // Frame time (Update + Draw + Wait time)
        
        float timeCounter = 0.0f;           // Accumulative time counter (seconds)
        float position = 0.0f;              // Circle position
        bool pause = false;                 // Pause control flag
        
        int targetFPS = 60;                 // Our initial target fps

        EngineInitialize();
        Initialize();

        ::SetConfigFlags(FLAG_MSAA_4X_HINT | FLAG_VSYNC_HINT | FLAG_WINDOW_RESIZABLE | FLAG_WINDOW_HIGHDPI);

        ::SetTargetFPS(TARGET_RENDER_FRAMERATE);
        ::InitWindow(screenWidth, screenHeight, windowName.c_str());
        ::rlImGuiSetup(true);

        ImGui::GetIO().ConfigFlags |= ImGuiConfigFlags_DockingEnable;
        
        EngineReady();
        Ready();
   
        while(!::WindowShouldClose()) {

#ifdef DN_DEBUG
            if (!debugIsGamePaused_) {
#endif /* DN_DEBUG */

            EnginePreFrame();


            // InputEvent e;
            PollInputEvents();
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

            ::BeginDrawing();
            ::rlImGuiBegin();

            ::ClearBackground(::Color{ 
                backgroundColor.r, 
                backgroundColor.g, 
                backgroundColor.b, 
                backgroundColor.a });

                ImGui::DockSpaceOverViewport(0,  NULL, ImGuiDockNodeFlags_PassthruCentralNode); // set ImGuiDockNodeFlags_PassthruCentralNode so that we can see the raylib contents behind the dockspace  

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


                ImGui::Begin("Another Window");   // Pass a pointer to our bool variable (the window will have a closing button that will clear the bool when clicked)
                ImGui::Text("Hello from another window!");
                ImGui::Button("Close Me");
                ImGui::End();

            // Update and Render additional Platform Windows
            ImGuiIO& io = ImGui::GetIO();
            if (io.ConfigFlags & ImGuiConfigFlags_ViewportsEnable)
            {
                ImGui::UpdatePlatformWindows();
                ImGui::RenderPlatformWindowsDefault();
                // TODO for OpenGL: restore current GL context.
            }

            EnginePostFrame();
#ifdef DN_DEBUG
            } 
#endif /* DN_DEBUG */
            ::rlImGuiEnd();
            ::EndDrawing();




            ::SwapScreenBuffer();
            currentTime = ::GetTime();
            updateDrawTime = currentTime - previousTime;
            
            if (targetFPS > 0)          // We want a fixed frame rate
            {
                waitTime = (1.0f/(float)targetFPS) - updateDrawTime;
                if (waitTime > 0.0) 
                {
                    ::WaitTime((float)waitTime);
                    currentTime = ::GetTime();
                    deltaTime = (float)(currentTime - previousTime);
                }
            }
            else deltaTime = (float)updateDrawTime;    // Framerate could be variable

            previousTime = currentTime;

#ifdef DN_DEBUG
                EngineDebug();
                Debug();
                EnginePostDebug();
#endif /* DN_DEBUG */

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

    void Application::EngineDebug()
    {
#ifdef DN_DEBUG
        if (IsKeyPressed(KEY_GRAVE)) {
            if (DebugIsGamePaused()) {
                DebugResumeGame();
            } else {
                DebugPauseGame();
            }
        }
#endif /* DN_DEBUG */
    }

    void Application::Debug()
    {
    }

    void Application::EnginePostDebug()
    {
        for (auto& callback : postDebugCallbacks) {
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
