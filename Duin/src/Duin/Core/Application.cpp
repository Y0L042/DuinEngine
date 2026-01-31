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

static bool USE_CUSTOM_IMGUI_INI_PATH = false;
static std::string IMGUI_INI_PATH = "./";

static bool PAUSE_ON_MINIMIZE = false;
static bool ALLOW_DOCKING_IN_MAIN = false;

// Color backgroundColor = WHITE;
// ::Camera3D activeCamera3D;

static SDL_Window *sdlWindow = NULL;
static SDL_Surface *sdlSurface = NULL;
static SDL_Renderer *sdlRenderer = NULL;

static SDL_WindowFlags sdlWindowFlags = 0;

static std::vector<std::function<void(void)>> postReadyCallbacks;
static std::vector<std::function<void(duin::Event)>> postInputCallbacks;
static std::vector<std::function<void(double)>> postUpdateCallbacks;
static std::vector<std::function<void(double)>> postPhysicsUpdateCallbacks;
static std::vector<std::function<void(void)>> postDrawCallbacks;
static std::vector<std::function<void(void)>> postDrawUICallbacks;
static std::vector<std::function<void(void)>> preFrameCallbacks;
static std::vector<std::function<void(void)>> postFrameCallbacks;
static std::vector<std::function<void(void)>> postDebugCallbacks;
static std::vector<std::function<void(void)>> exitCallbacks;

std::string duin::GetRootDirectory()
{
    std::filesystem::path currentPath = std::filesystem::current_path();
    return currentPath.string();
}

void duin::DebugPauseGame()
{
    debugIsGamePaused_ = 1;
}

void duin::DebugResumeGame()
{
    debugIsGamePaused_ = 0;
}

int duin::DebugIsGamePaused()
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

void duin::SetFramerate(int framerate)
{
    TARGET_RENDER_FRAMERATE = framerate;
}

void duin::DrawPhysicsFPS(float x, float y)
{
    static constexpr size_t bufferSize = 60; // Buffer size (approx. 1 second at 60 FPS)
    static std::array<float, bufferSize> physicsFPSBuffer = {};
    static size_t bufferIndex = 0;
    static float physicsFPSAverage = 0.0f;
    static float totalFPS = 0.0f; // Running total for quick average calculation

    // Get current frame's physics FPS
    float currentPhysicsFPS = duin::GetPhysicsFPS();

    // Update circular buffer
    totalFPS -= physicsFPSBuffer[bufferIndex];         // Remove old value from total
    physicsFPSBuffer[bufferIndex] = currentPhysicsFPS; // Add new value to buffer
    totalFPS += currentPhysicsFPS;                     // Add new value to total
    bufferIndex = (bufferIndex + 1) % bufferSize;      // Move to next index (circular)

    // Calculate average FPS
    physicsFPSAverage = totalFPS / bufferSize;

    // Format and draw the average
    char buffer[16];
    std::snprintf(buffer, sizeof(buffer), "P: %.2f", physicsFPSAverage);
    // ::DrawText(buffer, x, y, 30, ::GREEN);
}

void duin::DrawRenderFPS(float x, float y)
{
    static constexpr size_t bufferSize = 60; // Buffer size (approx. 1 second at 60 FPS)
    static std::array<float, bufferSize> renderFPSBuffer = {};
    static size_t bufferIndex = 0;
    static float renderFPSAverage = 0.0f;
    static float totalFPS = 0.0f; // Running total for quick average calculation

    // Get current frame's render FPS
    float currentRenderFPS = duin::GetRenderFPS();

    // Update circular buffer
    totalFPS -= renderFPSBuffer[bufferIndex];        // Remove old value from total
    renderFPSBuffer[bufferIndex] = currentRenderFPS; // Add new value to buffer
    totalFPS += currentRenderFPS;                    // Add new value to total
    bufferIndex = (bufferIndex + 1) % bufferSize;    // Move to next index (circular)

    // Calculate average FPS
    renderFPSAverage = totalFPS / bufferSize;

    // Format and draw the average
    char buffer[16];
    std::snprintf(buffer, sizeof(buffer), "R: %.2f", renderFPSAverage);
    // ::DrawText(buffer, x, y, 30, ::GREEN);
}

float duin::GetPhysicsFPS()
{
    float fps = 1.0f / (float)physicsFrameTime;
    return fps;
}

float duin::GetPhysicsFrameTime()
{
    float frametime = (float)physicsFrameTime;
    return frametime;
}

float duin::GetRenderFPS()
{
    float fps = 0.0f;
    if (renderFrameTime < 0.00001)
    {
        fps = 9999.9f;
    }
    else
    {
        fps = 1.0f / (float)renderFrameTime;
    }
    return fps;
}

float duin::GetRenderFrameTime()
{
    float frametime = (float)renderFrameTime;
    return frametime;
}

size_t duin::GetPhysicsFrameCount()
{
    return physicsFrameCount;
}

size_t duin::GetRenderFrameCount()
{
    return renderFrameCount;
}

double duin::GetTicks()
{
    return ::SDL_GetTicks() / 1000.0;
}

double duin::GetTicksMilli()
{
    return (double)::SDL_GetTicks();
}
double duin::GetTicksNano()
{
    return (double)::SDL_GetTicksNS();
}

void duin::DelayProcess(float seconds)
{
    ::SDL_Delay((uint32_t)seconds * 1000);
}
void duin::DelayProcessMilli(float milliseconds)
{
    ::SDL_Delay((uint32_t)milliseconds);
}

int duin::GetWindowWidth()
{
    return WINDOW_WIDTH;
}

int duin::GetWindowHeight()
{
    return WINDOW_HEIGHT;
}

void duin::SetWindowResizable(bool enable)
{
    SDL_SetWindowResizable(sdlWindow, enable);
    sdlWindowFlags |= (SDL_WINDOW_RESIZABLE && enable);
}

void duin::MaximizeWindow()
{
    SDL_MaximizeWindow(sdlWindow);
}

void duin::MinimizeWindow()
{
    SDL_MinimizeWindow(sdlWindow);
}

void duin::SetPauseOnMinimized(bool enable)
{
    PAUSE_ON_MINIMIZE = enable;
}

void duin::SetAllowDockingInMain(bool enable)
{
    ALLOW_DOCKING_IN_MAIN = enable;
}

void duin::SetImGuiINIPath(const std::string &newPath)
{
    DN_CORE_INFO("Custom ImGui INI path set: {}", newPath.c_str());
    USE_CUSTOM_IMGUI_INI_PATH = true;
    IMGUI_INI_PATH = newPath;
    ImGui::GetIO().IniFilename = NULL;
    ImGui::LoadIniSettingsFromDisk(IMGUI_INI_PATH.c_str());
}

SDL_Window *duin::GetSDLWindow()
{
    return sdlWindow;
}

void duin::QueuePostReadyCallback(std::function<void(void)> f)
{
    postReadyCallbacks.push_back(f);
}

void duin::QueuePostInputCallback(std::function<void(Event)> f)
{
    postInputCallbacks.push_back(f);
}

void duin::QueuePostUpdateCallback(std::function<void(double)> f)
{
    postUpdateCallbacks.push_back(f);
}

void duin::QueuePostPhysicsUpdateCallback(std::function<void(double)> f)
{
    postPhysicsUpdateCallbacks.push_back(f);
}

void duin::QueuePostDrawCallback(std::function<void()> f)
{
    postDrawCallbacks.push_back(f);
}

void duin::QueuePostDrawUICallback(std::function<void()> f)
{
    postDrawUICallbacks.push_back(f);
}

void duin::QueuePreFrameCallback(std::function<void()> f)
{
    preFrameCallbacks.push_back(f);
}

void duin::QueuePostFrameCallback(std::function<void()> f)
{
    postFrameCallbacks.push_back(f);
}

void duin::QueuePostDebugCallback(std::function<void()> f)
{
    postDebugCallbacks.push_back(f);
}

void duin::QueueExitCallback(std::function<void()> f)
{
    exitCallbacks.push_back(f);
}

duin::Application::Application()
{
    rootGameObject = std::make_shared<duin::GameObject>();
}

duin::Application::~Application()
{
}

void duin::Application::SetWindowStartupSize(int width, int height)
{
    WINDOW_WIDTH = width;
    WINDOW_HEIGHT = height;
}

// void Application::SetBackgroundColor(::Color color)
// {
//     backgroundColor = color;
// }

void duin::Application::SetWindowName(const char *string)
{
    windowName = std::string(string);
}

void duin::Application::Run()
{
    const double MAX_ACCUMULATOR = 0.25;
    double physicsCurrentTime = duin::GetTicks();
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

    if (!::SDL_Init(SDL_INIT_VIDEO))
    {
        ::SDL_Log("SDL could not initialize! SDL error: %s\n", ::SDL_GetError());
    }
    else
    {
        sdlWindow = ::SDL_CreateWindow(windowName.c_str(), WINDOW_WIDTH, WINDOW_HEIGHT, sdlWindowFlags);
        if (sdlWindow == nullptr)
        {
            ::SDL_Log("Window could not be created! SDL error: %s\n", ::SDL_GetError());
        }
        else
        {
            sdlSurface = ::SDL_GetWindowSurface(sdlWindow);
        }
    }
    ::HWND hwnd = (::HWND)::SDL_GetPointerProperty(::SDL_GetWindowProperties(sdlWindow),
                                                   SDL_PROP_WINDOW_WIN32_HWND_POINTER, NULL);
    if (!hwnd)
    {
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
    bgfx::setViewClear(RENDER_3D_VIEWID, BGFX_CLEAR_COLOR | BGFX_CLEAR_DEPTH, 0x443355FF, 1.0f, 0);
    bgfx::setViewRect(RENDER_3D_VIEWID, 0, 0, WINDOW_WIDTH, WINDOW_HEIGHT);
    ImGui::CreateContext();
    ::ImGui_Implbgfx_Init(255);
    ::ImGui_ImplSDL3_InitForD3D(sdlWindow);
    ImGui::GetIO().ConfigFlags |= ImGuiConfigFlags_DockingEnable;

    duin::InitRenderer();
    duin::SetRenderContextAvailable(true);

    EngineReady();
    Ready();
    EnginePostReady();

    while (!gameShouldQuit)
    {

#ifdef DN_DEBUG
        if (!debugIsGamePaused_)
        {
#endif /* DN_DEBUG */

            /* Do not run game when minimized */
            if (PAUSE_ON_MINIMIZE && SDL_GetWindowFlags(sdlWindow) && SDL_WINDOW_MINIMIZED)
            {
                duin::DelayProcessMilli(100);
                continue;
            }

            frameStartTime = duin::GetTicks();

            EnginePreFrame();

            /* If custom ImGui path is set, manually save to memory/disk */
            if (USE_CUSTOM_IMGUI_INI_PATH && ImGui::GetIO().WantSaveIniSettings)
            {
                ImGui::SaveIniSettingsToDisk(IMGUI_INI_PATH.c_str());
            }

            ::SDL_Event e;
            ::SDL_zero(e);
            while (::SDL_PollEvent(&e))
            {
                duin::Input::ProcessSDLMouseEvent(e);
                duin::Input::ProcessSDLKeyboardEvent(e);
                duin::EventHandler::GetPolledEvent(e);
                ::ImGui_ImplSDL3_ProcessEvent(&e);
            }
            duin::Input::CacheCurrentKeyState();
            duin::Input::CacheCurrentMouseKeyState();
            duin::Input::UpdateMouseFrameDelta();
            gameShouldQuit = duin::EventHandler::Get().IsCloseRequested();

            EngineUpdate(deltaTime);
            Update(deltaTime);
            EnginePostUpdate(deltaTime);

            physicsCurrentTime = duin::GetTicks();
            physicsDeltaTime = physicsCurrentTime - physicsPreviousTime;
            physicsAccumTime += physicsDeltaTime;
            double physicsTimeStep = (1.0 / (double)TARGET_PHYSICS_FRAMERATE);
            while (physicsAccumTime >= physicsTimeStep)
            {
                physicsPreviousTime = physicsCurrentTime;
                physicsAccumTime -= physicsTimeStep;

                physicsFrameTime = physicsTimeStep;
                ++physicsFrameCount;

                EnginePhysicsUpdate(physicsDeltaTime);
                PhysicsUpdate(physicsDeltaTime);
                EnginePostPhysicsUpdate(physicsDeltaTime);
            } // End of Physics

            /* Skip rendering when minimized */
            if (SDL_GetWindowFlags(sdlWindow) && e.type == SDL_WINDOW_MINIMIZED)
            {
                ImGui::Render();
                duin::DelayProcessMilli(100);
                continue;
            }

            // Update render rect on window resizing
            int displayWidth, displayHeight;
            ::SDL_GetWindowSize(sdlWindow, &displayWidth, &displayHeight);
            /* Skip rendering if window size is 0 */
            if (!(displayWidth && displayHeight))
            {
                duin::DelayProcessMilli(100);
                continue;
            }

            WINDOW_WIDTH = displayWidth;
            WINDOW_HEIGHT = displayHeight;

            bgfx::reset((uint32_t)displayWidth, (uint32_t)displayHeight, BGFX_RESET_VSYNC);
            bgfx::setViewRect(RENDER_3D_VIEWID, 0, 0, bgfx::BackbufferRatio::Equal);
            bgfx::touch(RENDER_3D_VIEWID);

            ++renderFrameCount;
            ::ImGui_Implbgfx_NewFrame();
            ::ImGui_ImplSDL3_NewFrame();
            ImGui::NewFrame();

            if (ALLOW_DOCKING_IN_MAIN)
            {
                const ImGuiDockNodeFlags dockspace_flags = ImGuiDockNodeFlags_PassthruCentralNode;
                ImGui::DockSpaceOverViewport(0, nullptr, dockspace_flags);
            }

            duin::BeginDraw3D(*duin::GetActiveCamera());

            EngineDraw();
            Draw();
            EnginePostDraw();

            EngineDrawUI();
            DrawUI();
            EnginePostDrawUI();

            duin::EndDraw3D();

            ImGui::Render();
            ::ImGui_Implbgfx_RenderDrawLists(ImGui::GetDrawData());
            duin::ExecuteRenderPipeline();

            EnginePostFrame();

            frameEndTime = duin::GetTicks();
            deltaDrawTime = frameEndTime - frameStartTime;

            if (TARGET_RENDER_FRAMERATE > 0)
            {
                double targetFrameTime = 1.0 / (double)TARGET_RENDER_FRAMERATE;
                waitTime = targetFrameTime - deltaDrawTime;

                // printf("Target FT:\t %.6f\n", targetFrameTime);
                // printf("deltaDrawTime:\t %.6f\n", deltaDrawTime);
                // printf("waittime:\t %.6f\n", waitTime);

                if (waitTime > 0.0)
                {
                    duin::DelayProcess((float)waitTime);
                }
            }

            deltaTime = duin::GetTicks() - frameStartTime;
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

    DN_CORE_INFO("Shutting down Rendering dependencies...");
    duin::SetRenderContextAvailable(false);

    ::ImGui_ImplSDL3_Shutdown();
    ::ImGui_Implbgfx_Shutdown();

    ImGui::DestroyContext();
    bgfx::shutdown();
    DN_CORE_INFO("ImGui context destroyed, bgfx shut down...");

    ::SDL_DestroySurface(sdlSurface);
    sdlSurface = nullptr;
    ::SDL_DestroyWindow(sdlWindow);
    sdlWindow = nullptr;
    DN_CORE_INFO("Quitting...");
    ::SDL_Quit();
}

void duin::Application::EngineInitialize()
{
    duin::EventHandler::Get().RegisterInputEventListener([this](duin::Event e) { EngineOnEvent(e); });
    duin::EventHandler::Get().RegisterInputEventListener([this](duin::Event e) { OnEvent(e); });
}

void duin::Application::Initialize()
{
}

void duin::Application::EngineReady()
{
}

void duin::Application::Ready()
{
}

void duin::Application::EnginePostReady()
{
    rootGameObject->ObjectReady();
    for (auto &callback : postReadyCallbacks)
    {
        callback();
    }
}

void duin::Application::EnginePreFrame()
{
    for (auto &callback : preFrameCallbacks)
    {
        callback();
    }
}

void duin::Application::EngineOnEvent(Event e)
{
    if (duin::Input::IsKeyDown(DN_KEY_ESCAPE))
    {
        DN_CORE_INFO("Quit event called... {}", e.sdlEvent.key.key);
        gameShouldQuit = true;
    }

    rootGameObject->ObjectOnEvent(e);
}

void duin::Application::OnEvent(Event e)
{
}

void duin::Application::EngineUpdate(double delta)
{
}

void duin::Application::Update(double delta)
{
}

void duin::Application::EnginePostUpdate(double delta)
{
    rootGameObject->ObjectUpdate(delta);
    for (auto &callback : postUpdateCallbacks)
    {
        callback(delta);
    }
}

void duin::Application::EnginePhysicsUpdate(double delta)
{
}

void duin::Application::PhysicsUpdate(double delta)
{
}

void duin::Application::EnginePostPhysicsUpdate(double delta)
{
    rootGameObject->ObjectPhysicsUpdate(delta);
    for (auto &callback : postPhysicsUpdateCallbacks)
    {
        callback(delta);
    }

    duin::PhysicsServer::Get().StepPhysics(delta);
}

void duin::Application::EngineDraw()
{
}

void duin::Application::Draw()
{
}

void duin::Application::EnginePostDraw()
{
    rootGameObject->ObjectDraw();
    for (auto &callback : postDrawCallbacks)
    {
        callback();
    }
}

void duin::Application::EngineDrawUI()
{
}

void duin::Application::DrawUI()
{
}

void duin::Application::EnginePostDrawUI()
{
    rootGameObject->ObjectDrawUI();
    for (auto &callback : postDrawUICallbacks)
    {
        callback();
    }
}

void duin::Application::EngineDebug()
{
#ifdef DN_DEBUG
    // if (IsKeyPressed(KEY_GRAVE)) {
    //     if (DebugIsGamePaused()) {
    //  DebugResumeGame();
    //     } else {
    //         DebugPauseGame();
    //   }
    // }
#endif /* DN_DEBUG */
}

void duin::Application::Debug()
{
}

void duin::Application::EnginePostDebug()
{
    rootGameObject->ObjectDebug();
    for (auto &callback : postDebugCallbacks)
    {
        callback();
    }
}

void duin::Application::EnginePostFrame()
{
    for (auto &callback : postFrameCallbacks)
    {
        callback();
    }
}

void duin::Application::EngineExit()
{
    for (auto &callback : exitCallbacks)
    {
        callback();
    }
}

void duin::Application::Exit()
{
}

void duin::Application::AddChildObject(std::shared_ptr<GameObject> child)
{
    rootGameObject->AddChildObject(child);
}

void duin::Application::RemoveChildObject(std::shared_ptr<GameObject> child)
{
    rootGameObject->RemoveChildObject(child);
}
