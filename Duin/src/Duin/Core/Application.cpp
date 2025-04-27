#include "dnpch.h"
#include "Application.h"

#include "Duin/Core/Events/EventHandler.h"
#include "Duin/Core/Events/Keys.h"
#include "Duin/Core/Events/EngineInput.h"
#include "Duin/Core/Events/Input.h"
#include "Duin/Physics/PhysicsModule.h"
#include "Duin/Render/Renderer.h"


#define SDL_MAIN_HANDLED
#include <SDL3/SDL.h>
#include <SDL3/SDL_main.h>

#ifndef BX_CONFIG_DEBUG
#define BX_CONFIG_DEBUG 0
#endif
#include <bx/bx.h>
#include <bx/math.h>

#include <bgfx/bgfx.h>
#include <bgfx/platform.h>

#include "external/backends/imgui_impl_bgfx.h"
#include "external/backends/imgui_impl_sdl3.h"


#include "Duin/Render/RenderShape.h"
#include "Duin/Render/Shader.h"

#include <filesystem>


static int debugIsGamePaused_ = 0;

static bool gameShouldQuit = false;

static size_t physicsFrameCount = 0;
static size_t renderFrameCount = 0;

static double physicsFrameTime = 0.0;
static double renderFrameTime = 0.0;

static int TARGET_RENDER_FRAMERATE = 60;
static int TARGET_PHYSICS_FRAMERATE = 60;

static int WINDOW_WIDTH = 1280;
static int WINDOW_HEIGHT = 720;

static bool PAUSE_ON_MINIMIZE = false;
static bool ALLOW_DOCKING_IN_MAIN = false;

// Color backgroundColor = WHITE;
// ::Camera3D activeCamera3D;

static SDL_Window *sdlWindow = NULL;
static SDL_Surface* sdlSurface = NULL;
static SDL_Renderer *sdlRenderer = NULL;

static SDL_WindowFlags sdlWindowFlags = 0;

const bgfx::ViewId MAIN_DISPLAY = 0;



namespace duin {

    static std::vector<std::function<void(void)>> postReadyCallbacks;
    static std::vector<std::function<void(Event)>> postInputCallbacks;
    static std::vector<std::function<void(double)>> postUpdateCallbacks;
    static std::vector<std::function<void(double)>> postPhysicsUpdateCallbacks;
    static std::vector<std::function<void(void)>> postDrawCallbacks;
    static std::vector<std::function<void(void)>> postDrawUICallbacks;
    static std::vector<std::function<void(void)>> preFrameCallbacks;
    static std::vector<std::function<void(void)>> postFrameCallbacks;
    static std::vector<std::function<void(void)>> postDebugCallbacks;

    std::string GetRootDirectory()
    {
        std::filesystem::path currentPath = std::filesystem::current_path();
        return currentPath.string();
    }

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

    // void SetActiveCamera3D(::Camera3D camera3d)
    // {
    //     activeCamera3D = camera3d;
    // }

    // void SetBackgroundColor(::Color color)
    // {
    //     backgroundColor = color;
    // }

    void SetFramerate(int framerate)
    {
        TARGET_RENDER_FRAMERATE = framerate;
    }

    void DrawPhysicsFPS(float x, float y) {
        static constexpr size_t bufferSize = 60; // Buffer size (approx. 1 second at 60 FPS)
        static std::array<float, bufferSize> physicsFPSBuffer = {};
        static size_t bufferIndex = 0;
        static float physicsFPSAverage = 0.0f;
        static float totalFPS = 0.0f; // Running total for quick average calculation

        // Get current frame's physics FPS
        float currentPhysicsFPS = GetPhysicsFPS();

        // Update circular buffer
        totalFPS -= physicsFPSBuffer[bufferIndex];     // Remove old value from total
        physicsFPSBuffer[bufferIndex] = currentPhysicsFPS; // Add new value to buffer
        totalFPS += currentPhysicsFPS;                // Add new value to total
        bufferIndex = (bufferIndex + 1) % bufferSize; // Move to next index (circular)

        // Calculate average FPS
        physicsFPSAverage = totalFPS / bufferSize;

        // Format and draw the average
        char buffer[16];
        std::snprintf(buffer, sizeof(buffer), "P: %.2f", physicsFPSAverage);
        // ::DrawText(buffer, x, y, 30, ::GREEN);
    }

    void DrawRenderFPS(float x, float y) {
        static constexpr size_t bufferSize = 60; // Buffer size (approx. 1 second at 60 FPS)
        static std::array<float, bufferSize> renderFPSBuffer = {};
        static size_t bufferIndex = 0;
        static float renderFPSAverage = 0.0f;
        static float totalFPS = 0.0f; // Running total for quick average calculation

        // Get current frame's render FPS
        float currentRenderFPS = GetRenderFPS();

        // Update circular buffer
        totalFPS -= renderFPSBuffer[bufferIndex];     // Remove old value from total
        renderFPSBuffer[bufferIndex] = currentRenderFPS; // Add new value to buffer
        totalFPS += currentRenderFPS;                // Add new value to total
        bufferIndex = (bufferIndex + 1) % bufferSize; // Move to next index (circular)

        // Calculate average FPS
        renderFPSAverage = totalFPS / bufferSize;

        // Format and draw the average
        char buffer[16];
        std::snprintf(buffer, sizeof(buffer), "R: %.2f", renderFPSAverage);
        // ::DrawText(buffer, x, y, 30, ::GREEN);
    }

    float GetPhysicsFPS()
    {
        float fps = 1.0f / (float)physicsFrameTime;
        return fps;
    }

    float GetPhysicsFrameTime()
    {
        float frametime = (float)physicsFrameTime;
        return frametime;
    }

    float GetRenderFPS()
    {
        float fps = 0.0f;
        if (renderFrameTime < 0.00001) {
            fps = 9999.9f;
        } else {
            fps = 1.0f / (float)renderFrameTime;
        }
        return fps;
    }

    float GetRenderFrameTime()
    {
        float frametime = (float)renderFrameTime;
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

    double GetTicks()
    {
        return ::SDL_GetTicks() / 1000.0;
    }

    double GetTicksMilli()
    {
        return (double)::SDL_GetTicks();
    }
    double GetTicksNano()
    {
        return (double)::SDL_GetTicksNS();
    }

    void DelayProcess(float seconds)
    {
        ::SDL_Delay((uint32_t)seconds * 1000);
    }
    void DelayProcessMilli(float milliseconds)
    {
        ::SDL_Delay((uint32_t)milliseconds);
    }

    int GetWindowWidth()
    {
        return WINDOW_WIDTH;
    }

    int GetWindowHeight()
    {
        return WINDOW_HEIGHT;
    }

    void SetWindowResizable(bool enable)
    {
        SDL_SetWindowResizable(sdlWindow, enable);
        sdlWindowFlags |= (SDL_WINDOW_RESIZABLE & enable);
    }

    void MaximizeWindow()
    {
        SDL_MaximizeWindow(sdlWindow);
    }

    void MinimizeWindow()
    {
        SDL_MinimizeWindow(sdlWindow);
    }

    void SetPauseOnMinimized(bool enable)
    {
        PAUSE_ON_MINIMIZE = enable;
    }

    void SetAllowDockingInMain(bool enable)
    {
        ALLOW_DOCKING_IN_MAIN = enable;
    }



    SDL_Window* GetSDLWindow()
    {
        return sdlWindow;
    }

    void QueuePostReadyCallback(std::function<void(void)> f)
    {
        postReadyCallbacks.push_back(f);
    }

    void QueuePostInputCallback(std::function<void(Event)> f)
    {
        postInputCallbacks.push_back(f);
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
        rootGameObject = std::make_shared<GameObject>();
    }

    Application::~Application()
    {
    }

    void Application::SetWindowStartupSize(int width, int height)
    {
        WINDOW_WIDTH = width;
        WINDOW_HEIGHT = height;
    }

    // void Application::SetBackgroundColor(::Color color)
    // {
    //     backgroundColor = color;
    // }

    void Application::SetWindowName(const char* string)
    {
        windowName = std::string(string);
    }





    void Application::Run()
    {    
        const double MAX_ACCUMULATOR = 0.25;
        double physicsCurrentTime = GetTicks();
        double physicsPreviousTime = 0.0;
        double physicsDeltaTime = 0.0;
        double physicsAccumTime = 0.0;

        double frameEndTime = 0.0;
        double frameStartTime = 0.0;
        double deltaDrawTime = 0.0;
        double waitTime = 0.0;
        double deltaTime = 0.0;


        EngineInitialize();
        Initialize();

        DN_CORE_INFO("Set render FPS {}", TARGET_RENDER_FRAMERATE);

        if(!::SDL_Init(SDL_INIT_VIDEO)) {
            ::SDL_Log("SDL could not initialize! SDL error: %s\n", ::SDL_GetError());
        } else {
            sdlWindow = ::SDL_CreateWindow(windowName.c_str(), WINDOW_WIDTH, WINDOW_HEIGHT, sdlWindowFlags);
            if(sdlWindow == nullptr) {
                ::SDL_Log("Window could not be created! SDL error: %s\n", ::SDL_GetError());
            } else {
                sdlSurface = ::SDL_GetWindowSurface(sdlWindow);
            }
        }
        ::HWND hwnd = (::HWND)::SDL_GetPointerProperty(::SDL_GetWindowProperties(sdlWindow), SDL_PROP_WINDOW_WIN32_HWND_POINTER, NULL);
        if (!hwnd) {
            DN_CORE_FATAL("SDL3 window handle not found!");
        }
        bgfx::renderFrame();
        bgfx::Init bgfxInit;
        bgfxInit.type = bgfx::RendererType::Count; // Automatically choose a renderer
        bgfxInit.resolution.width = WINDOW_WIDTH;
        bgfxInit.resolution.height = WINDOW_HEIGHT;
        bgfxInit.resolution.reset = BGFX_RESET_VSYNC;
        bgfxInit.platformData.nwh = hwnd;
        bgfx::init(bgfxInit);
        bgfx::setViewClear(MAIN_DISPLAY, BGFX_CLEAR_COLOR | BGFX_CLEAR_DEPTH, 0x443355FF, 1.0f, 0);
        bgfx::setViewRect(MAIN_DISPLAY, 0, 0, WINDOW_WIDTH, WINDOW_HEIGHT);
        ImGui::CreateContext();
        ::ImGui_Implbgfx_Init(255);
        ::ImGui_ImplSDL3_InitForD3D(sdlWindow);
        ImGui::GetIO().ConfigFlags |= ImGuiConfigFlags_DockingEnable;

        InitRenderer();

        EngineReady();
        Ready();
        EnginePostReady();

        while(!gameShouldQuit) {

            #ifdef DN_DEBUG
                if (!debugIsGamePaused_) {
            #endif /* DN_DEBUG */

            /* Do not run game when minimized */
            if (PAUSE_ON_MINIMIZE && SDL_GetWindowFlags(sdlWindow) && SDL_WINDOW_MINIMIZED) {
                DelayProcessMilli(100);
                continue;
            }

            frameStartTime = GetTicks();

            EnginePreFrame();

            ::SDL_Event e;
            ::SDL_zero(e);
            while (::SDL_PollEvent(&e)) {
                Input::ProcessSDLMouseEvent(e);
                Input::ProcessSDLKeyboardEvent(e);
                EventHandler::GetPolledEvent(e);
                ::ImGui_ImplSDL3_ProcessEvent(&e);
            }
            Input::CacheCurrentKeyState();
            Input::UpdateMouseFrameDelta();

            EngineUpdate(deltaTime);
            Update(deltaTime);
            EnginePostUpdate(deltaTime);

            physicsCurrentTime = GetTicks();
            physicsDeltaTime = physicsCurrentTime - physicsPreviousTime;
            physicsAccumTime += physicsDeltaTime;
            double physicsTimeStep = (1.0 / (double)TARGET_PHYSICS_FRAMERATE);
            while (physicsAccumTime >= physicsTimeStep) {
                physicsPreviousTime = physicsCurrentTime;
                physicsAccumTime -= physicsTimeStep;

                physicsFrameTime = physicsTimeStep;
                ++physicsFrameCount;


                EnginePhysicsUpdate(physicsDeltaTime);
                PhysicsUpdate(physicsDeltaTime); 
                EnginePostPhysicsUpdate(physicsDeltaTime);
            } // End of Physics

            /* Skip rendering when minimized */
            if (SDL_GetWindowFlags(sdlWindow) && e.type == SDL_WINDOW_MINIMIZED) {
                ImGui::Render();
                DelayProcessMilli(100);
                continue;
            }

            
            // Update render rect on window resizing
            int displayWidth, displayHeight;
            ::SDL_GetWindowSize(sdlWindow, &displayWidth, &displayHeight);
            /* Skip rendering if window size is 0 */
            if (!(displayWidth && displayHeight)) {
                DelayProcessMilli(100);
                continue;
            }

            bgfx::reset((uint32_t)displayWidth, (uint32_t)displayHeight, BGFX_RESET_VSYNC);
            bgfx::setViewRect(MAIN_DISPLAY, 0, 0, bgfx::BackbufferRatio::Equal);
            bgfx::touch(MAIN_DISPLAY);

            ++renderFrameCount;
            ::ImGui_Implbgfx_NewFrame();
            ::ImGui_ImplSDL3_NewFrame();
            ImGui::NewFrame();

            if (ALLOW_DOCKING_IN_MAIN) {
                const ImGuiDockNodeFlags dockspace_flags = ImGuiDockNodeFlags_PassthruCentralNode;
                ImGui::DockSpaceOverViewport(0, nullptr, dockspace_flags);
            }

            BeginDraw3D(*GetActiveCamera());

                EngineDraw();
                Draw();
                EnginePostDraw();

                EngineDrawUI();
                DrawUI();
                EnginePostDrawUI();

            EndDraw3D();

            ImGui::Render();
            ::ImGui_Implbgfx_RenderDrawLists(ImGui::GetDrawData());
            ExecuteRenderPipeline();

            EnginePostFrame();

            frameEndTime = GetTicks();
            deltaDrawTime = frameEndTime - frameStartTime;

            if (TARGET_RENDER_FRAMERATE > 0) {
                double targetFrameTime = 1.0 / (double)TARGET_RENDER_FRAMERATE;
                waitTime = targetFrameTime - deltaDrawTime;

                // printf("Target FT:\t %.6f\n", targetFrameTime);
                // printf("deltaDrawTime:\t %.6f\n", deltaDrawTime);
                // printf("waittime:\t %.6f\n", waitTime);

                if (waitTime > 0.0) {
                    DelayProcess((float)waitTime);
                }
            } 

            deltaTime = GetTicks() - frameStartTime; 
            renderFrameTime = deltaTime;


            #ifdef DN_DEBUG
                } 
                EngineDebug();
                Debug();
                EnginePostDebug();
            #endif /* DN_DEBUG */

        } // End of Application While

        EngineExit();
        Exit();

        ::ImGui_ImplSDL3_Shutdown();
        ::ImGui_Implbgfx_Shutdown();

        ImGui::DestroyContext();
        bgfx::shutdown();

        ::SDL_DestroySurface(sdlSurface);
        sdlSurface = nullptr;
        ::SDL_DestroyWindow(sdlWindow);
        sdlWindow = nullptr;
        ::SDL_Quit();
    }





    void Application::EngineInitialize()
    {
        EventHandler::Get().RegisterInputEventListener([this](Event e){ EngineOnEvent(e); });
        EventHandler::Get().RegisterInputEventListener([this](Event e){ OnEvent(e); });
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

    void Application::EnginePostReady()
    {
        rootGameObject->ObjectReady();
        for (auto& callback : postReadyCallbacks) {
            callback();
        }
    }

    void Application::EnginePreFrame()
    {
        for (auto& callback : preFrameCallbacks) {
            callback();
        }
    }

    void Application::EngineOnEvent(Event e)
    {
        if (Input::IsKeyDown(DN_KEY_ESCAPE)) {
            DN_CORE_INFO("Quiting... {}", e.sdlEvent.key.key);
            gameShouldQuit = true;
        }

        rootGameObject->ObjectOnEvent(e);
    }

    void Application::OnEvent(Event e)
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
        rootGameObject->ObjectUpdate(delta);
        for (auto& callback : postUpdateCallbacks) {
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
        rootGameObject->ObjectPhysicsUpdate(delta);
        for (auto& callback : postPhysicsUpdateCallbacks) {
            callback(delta);
        }

        PhysicsServer::Get().StepPhysics(delta);
    }

    void Application::EngineDraw()
    {
    }

    void Application::Draw()
    {
    }

    void Application::EnginePostDraw()
    {
        rootGameObject->ObjectDraw();
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
        rootGameObject->ObjectDrawUI();
        for (auto& callback : postDrawUICallbacks) {
            callback();
        }
    }

    void Application::EngineDebug()
    {
#ifdef DN_DEBUG
        // if (IsKeyPressed(KEY_GRAVE)) {
        //     if (DebugIsGamePaused()) {
        //         DebugResumeGame();
        //     } else {
        //         DebugPauseGame();
        //     }
        // }
#endif /* DN_DEBUG */
    }

    void Application::Debug()
    {
    }

    void Application::EnginePostDebug()
    {
        rootGameObject->ObjectDebug();
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

    void Application::AddChild(std::shared_ptr<GameObject> child)
    {
        rootGameObject->AddChild(child);
    }

    void Application::RemoveChild(std::shared_ptr<GameObject> child)
    {
        rootGameObject->RemoveChild(child);
    }

}
