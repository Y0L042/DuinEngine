#include "dnpch.h"
#include "Application.h"

#include "Duin/Core/Events/EventHandler.h"
#include "Duin/Core/Events/Keys.h"
#include "Duin/Core/Events/EngineInput.h"
#include "Duin/Core/Events/Input.h"
#include "Duin/Render/Renderer.h"

#define SDL_MAIN_HANDLED

#ifndef BX_CONFIG_DEBUG
#define BX_CONFIG_DEBUG 0
#endif

#include <bgfx/bgfx.h>
#include <bgfx/platform.h>

#include "external/backends/imgui_impl_bgfx.h"
#include "external/backends/imgui_impl_sdl3.h"


#include <filesystem>
#include <Windows.h>
#include <array>
#include <cstdint>
#include <cstdio>
#include <functional>
#include <memory>
#include <string>
#include <utility>
#include <DEBUG_DEFINES.h>
#include "Debug/DNLog.h"
#include "Events/Event.h"
#include "Signals/Signal.h"
#include <Duin/Objects/GameObject.h>
#include <Duin/Physics/physx/PhysicsServer.h>
#include <Duin/Render/Camera.h>
#include <external/imgui.h>
#include <bgfx/defines.h>
#include <SDL3/SDL_error.h>
#include <SDL3/SDL_events.h>
#include <SDL3/SDL_init.h>
#include <SDL3/SDL_log.h>
#include <SDL3/SDL_properties.h>
#include <SDL3/SDL_render.h>
#include <SDL3/SDL_stdinc.h>
#include <SDL3/SDL_surface.h>
#include <SDL3/SDL_timer.h>
#include <SDL3/SDL_video.h>

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

static duin::Signal<> postReadySignal;
static duin::Signal<duin::Event> postInputSignal;
static duin::Signal<double> postUpdateSignal;
static duin::Signal<double> postPhysicsUpdateSignal;
static duin::Signal<> postDrawSignal;
static duin::Signal<> postDrawUISignal;
static duin::Signal<> preFrameSignal;
static duin::Signal<> postFrameSignal;
static duin::Signal<> postDebugSignal;
static duin::Signal<> exitSignal;

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

std::shared_ptr<duin::ScopedConnection> duin::QueuePostReadyCallback(std::function<void(void)> f)
{
    return postReadySignal.ConnectScoped(std::move(f));
}

std::shared_ptr<duin::ScopedConnection> duin::QueuePostInputCallback(std::function<void(Event)> f)
{
    return postInputSignal.ConnectScoped(std::move(f));
}

std::shared_ptr<duin::ScopedConnection> duin::QueuePostUpdateCallback(std::function<void(double)> f)
{
    return postUpdateSignal.ConnectScoped(std::move(f));
}

std::shared_ptr<duin::ScopedConnection> duin::QueuePostPhysicsUpdateCallback(std::function<void(double)> f)
{
    return postPhysicsUpdateSignal.ConnectScoped(std::move(f));
}

std::shared_ptr<duin::ScopedConnection> duin::QueuePostDrawCallback(std::function<void()> f)
{
    return postDrawSignal.ConnectScoped(std::move(f));
}

std::shared_ptr<duin::ScopedConnection> duin::QueuePostDrawUICallback(std::function<void()> f)
{
    return postDrawUISignal.ConnectScoped(std::move(f));
}

std::shared_ptr<duin::ScopedConnection> duin::QueuePreFrameCallback(std::function<void()> f)
{
    return preFrameSignal.ConnectScoped(std::move(f));
}

std::shared_ptr<duin::ScopedConnection> duin::QueuePostFrameCallback(std::function<void()> f)
{
    return postFrameSignal.ConnectScoped(std::move(f));
}

std::shared_ptr<duin::ScopedConnection> duin::QueuePostDebugCallback(std::function<void()> f)
{
    return postDebugSignal.ConnectScoped(std::move(f));
}

std::shared_ptr<duin::ScopedConnection> duin::QueueExitCallback(std::function<void()> f)
{
    return exitSignal.ConnectScoped(std::move(f));
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

#ifdef DN_HEADLESS
void duin::Application::SetHeadless(bool headless)
{
    headlessMode = headless;
}
#endif /* DN_HEADLESS */

void duin::Application::Run()
{
    double physicsCurrentTime = duin::GetTicks();
    double physicsPreviousTime = 0.0;
    double physicsAccumTime = 0.0;
    double deltaTime = 0.0;

    EngineInitialize();
    Initialize();

    DN_CORE_INFO("Set render FPS {}", TARGET_RENDER_FRAMERATE);

    InitSDL();
    InitBGFX();
    InitImGui();

#ifdef DN_HEADLESS
    if (!headlessMode)
    {
        duin::InitRenderer();
        duin::SetRenderContextAvailable(true);
    }
#else
    duin::InitRenderer();
    duin::SetRenderContextAvailable(true);
#endif /* DN_HEADLESS */

    EngineReady();
    Ready();
    EnginePostReady();

    while (ProcessFrame(deltaTime, physicsCurrentTime, physicsPreviousTime, physicsAccumTime))
    {
    }

    EngineExit();
    Exit();

    DN_CORE_INFO("Shutting down Rendering dependencies...");

#ifdef DN_HEADLESS
    if (!headlessMode)
        duin::SetRenderContextAvailable(false);
#else
    duin::SetRenderContextAvailable(false);
#endif /* DN_HEADLESS */

    ShutdownImGui();
    ShutdownBGFX();
    ShutdownSDL();
}

void duin::Application::InitSDL()
{
#ifdef DN_HEADLESS
    if (headlessMode)
    {
        if (!::SDL_Init(SDL_INIT_EVENTS))
            ::SDL_Log("SDL (events) could not initialize! SDL error: %s\n", ::SDL_GetError());
        return;
    }
#endif /* DN_HEADLESS */

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
}

void duin::Application::InitBGFX()
{
    bgfx::Init bgfxInit;
    bgfxInit.type = bgfx::RendererType::Count; // Automatically choose a renderer

#ifdef DN_HEADLESS
    if (headlessMode)
    {
        bgfxInit.resolution.width = 0;
        bgfxInit.resolution.height = 0;
        bgfxInit.resolution.reset = BGFX_RESET_NONE;
        bgfxInit.platformData.nwh = nullptr;
    }
    else
    {
        bgfx::renderFrame(); // Claim render thread before bgfx::init() spawns one
        ::HWND hwnd = (::HWND)::SDL_GetPointerProperty(::SDL_GetWindowProperties(sdlWindow),
                                                       SDL_PROP_WINDOW_WIN32_HWND_POINTER, NULL);
        if (!hwnd)
        {
            DN_CORE_FATAL("SDL3 window handle not found!");
        }
        bgfxInit.resolution.width = WINDOW_WIDTH;
        bgfxInit.resolution.height = WINDOW_HEIGHT;
        bgfxInit.resolution.reset = BGFX_RESET_VSYNC;
        bgfxInit.platformData.nwh = hwnd;
    }
#else
    bgfx::renderFrame(); // Claim render thread before bgfx::init() spawns one
    ::HWND hwnd = (::HWND)::SDL_GetPointerProperty(::SDL_GetWindowProperties(sdlWindow),
                                                   SDL_PROP_WINDOW_WIN32_HWND_POINTER, NULL);
    if (!hwnd)
    {
        DN_CORE_FATAL("SDL3 window handle not found!");
    }
    bgfxInit.resolution.width = WINDOW_WIDTH;
    bgfxInit.resolution.height = WINDOW_HEIGHT;
    bgfxInit.resolution.reset = BGFX_RESET_VSYNC;
    bgfxInit.platformData.nwh = hwnd;
#endif /* DN_HEADLESS */

    bgfx::init(bgfxInit);

#ifdef DN_HEADLESS
    if (!headlessMode)
    {
        bgfx::setViewClear(RENDER_3D_VIEWID, BGFX_CLEAR_COLOR | BGFX_CLEAR_DEPTH, 0x443355FF, 1.0f, 0);
        bgfx::setViewRect(RENDER_3D_VIEWID, 0, 0, WINDOW_WIDTH, WINDOW_HEIGHT);
    }
#else
    bgfx::setViewClear(RENDER_3D_VIEWID, BGFX_CLEAR_COLOR | BGFX_CLEAR_DEPTH, 0x443355FF, 1.0f, 0);
    bgfx::setViewRect(RENDER_3D_VIEWID, 0, 0, WINDOW_WIDTH, WINDOW_HEIGHT);
#endif /* DN_HEADLESS */
}

void duin::Application::InitImGui()
{
#ifdef DN_HEADLESS
    if (headlessMode)
        return;
#endif /* DN_HEADLESS */

    ImGui::CreateContext();
    ::ImGui_Implbgfx_Init(255);
    ::ImGui_ImplSDL3_InitForD3D(sdlWindow);
    ImGui::GetIO().ConfigFlags |= ImGuiConfigFlags_DockingEnable;
}

void duin::Application::ShutdownImGui()
{
#ifdef DN_HEADLESS
    if (headlessMode)
        return;
#endif /* DN_HEADLESS */

    ::ImGui_ImplSDL3_Shutdown();
    ::ImGui_Implbgfx_Shutdown();
    ImGui::DestroyContext();
}

void duin::Application::ShutdownBGFX()
{
    bgfx::shutdown();
    DN_CORE_INFO("bgfx shut down...");
}

void duin::Application::ShutdownSDL()
{
#ifdef DN_HEADLESS
    if (!headlessMode)
    {
        ::SDL_DestroySurface(sdlSurface);
        sdlSurface = nullptr;
        ::SDL_DestroyWindow(sdlWindow);
        sdlWindow = nullptr;
    }
#else
    ::SDL_DestroySurface(sdlSurface);
    sdlSurface = nullptr;
    ::SDL_DestroyWindow(sdlWindow);
    sdlWindow = nullptr;
#endif /* DN_HEADLESS */

    DN_CORE_INFO("Quitting...");
    ::SDL_Quit();
}

#ifdef DN_TESTING
void duin::Application::InitRenderer()
{
    duin::InitRenderer();
    duin::SetRenderContextAvailable(true);
}

void duin::Application::CleanRenderer()
{
    duin::ResetRenderer();
}

void duin::Application::BeginRenderFrame()
{
    duin::BeginEncoderFrame();
}

void duin::Application::EndRenderFrame()
{
    duin::EndEncoderFrame();
}
#endif /* DN_TESTING */

void duin::Application::ProcessEvents()
{
    ::SDL_Event e;
    ::SDL_zero(e);
    while (::SDL_PollEvent(&e))
    {
        duin::Input::ProcessSDLMouseEvent(e);
        duin::Input::ProcessSDLKeyboardEvent(e);
        duin::EventHandler::GetPolledEvent(e);

#ifdef DN_HEADLESS
        if (!headlessMode)
            ::ImGui_ImplSDL3_ProcessEvent(&e);
#else
        ::ImGui_ImplSDL3_ProcessEvent(&e);
#endif /* DN_HEADLESS */
    }
    duin::Input::CacheCurrentKeyState();
    duin::Input::CacheCurrentMouseKeyState();
    duin::Input::UpdateMouseFrameDelta();
    gameShouldQuit = duin::EventHandler::Get().IsCloseRequested();
}

void duin::Application::RunUpdate(double delta)
{
    EngineUpdate(delta);
    Update(delta);
    EnginePostUpdate(delta);
}

void duin::Application::RunPhysics(double &physicsCurrentTime, double &physicsPreviousTime, double &physicsAccumTime)
{
    physicsCurrentTime = duin::GetTicks();
    double physicsDeltaTime = physicsCurrentTime - physicsPreviousTime;
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
}

void duin::Application::RunRender()
{
#ifdef DN_HEADLESS
    if (headlessMode)
        return;
#endif /* DN_HEADLESS */

    // Update render rect on window resizing
    int displayWidth, displayHeight;
    ::SDL_GetWindowSize(sdlWindow, &displayWidth, &displayHeight);

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
}

bool duin::Application::ProcessFrame(double &deltaTime, double &physicsCurrentTime, double &physicsPreviousTime,
                                     double &physicsAccumTime)
{
#ifdef DN_DEBUG
    if (!debugIsGamePaused_)
    {
#endif /* DN_DEBUG */

#ifdef DN_HEADLESS
        if (!headlessMode)
        {
            /* Do not run game when minimized */
            if (PAUSE_ON_MINIMIZE && SDL_GetWindowFlags(sdlWindow) && SDL_WINDOW_MINIMIZED)
            {
                duin::DelayProcessMilli(100);
                return !gameShouldQuit;
            }
        }
#else
    /* Do not run game when minimized */
    if (PAUSE_ON_MINIMIZE && SDL_GetWindowFlags(sdlWindow) && SDL_WINDOW_MINIMIZED)
    {
        duin::DelayProcessMilli(100);
        return !gameShouldQuit;
    }
#endif /* DN_HEADLESS */

        double frameStartTime = duin::GetTicks();

        EnginePreFrame();

#ifdef DN_HEADLESS
        if (!headlessMode)
        {
            /* If custom ImGui path is set, manually save to memory/disk */
            if (USE_CUSTOM_IMGUI_INI_PATH && ImGui::GetIO().WantSaveIniSettings)
            {
                ImGui::SaveIniSettingsToDisk(IMGUI_INI_PATH.c_str());
            }
        }
#else
    /* If custom ImGui path is set, manually save to memory/disk */
    if (USE_CUSTOM_IMGUI_INI_PATH && ImGui::GetIO().WantSaveIniSettings)
    {
        ImGui::SaveIniSettingsToDisk(IMGUI_INI_PATH.c_str());
    }
#endif /* DN_HEADLESS */

        ProcessEvents();

        RunUpdate(deltaTime);

        RunPhysics(physicsCurrentTime, physicsPreviousTime, physicsAccumTime);

#ifdef DN_HEADLESS
        if (!headlessMode)
        {
            /* Skip rendering when minimized */
            SDL_Event e;
            SDL_zero(e);
            if (SDL_GetWindowFlags(sdlWindow) && e.type == SDL_WINDOW_MINIMIZED)
            {
                ImGui::Render();
                duin::DelayProcessMilli(100);
                return !gameShouldQuit;
            }

            /* Skip rendering if window size is 0 */
            int displayWidth, displayHeight;
            ::SDL_GetWindowSize(sdlWindow, &displayWidth, &displayHeight);
            if (!(displayWidth && displayHeight))
            {
                duin::DelayProcessMilli(100);
                return !gameShouldQuit;
            }
        }
#else
    /* Skip rendering when minimized */
    SDL_Event e;
    SDL_zero(e);
    if (SDL_GetWindowFlags(sdlWindow) && e.type == SDL_WINDOW_MINIMIZED)
    {
        ImGui::Render();
        duin::DelayProcessMilli(100);
        return !gameShouldQuit;
    }

    /* Skip rendering if window size is 0 */
    int displayWidth, displayHeight;
    ::SDL_GetWindowSize(sdlWindow, &displayWidth, &displayHeight);
    if (!(displayWidth && displayHeight))
    {
        duin::DelayProcessMilli(100);
        return !gameShouldQuit;
    }
#endif /* DN_HEADLESS */

        RunRender();

        EnginePostFrame();

        double frameEndTime = duin::GetTicks();
        double deltaDrawTime = frameEndTime - frameStartTime;

        if (TARGET_RENDER_FRAMERATE > 0)
        {
            double targetFrameTime = 1.0 / (double)TARGET_RENDER_FRAMERATE;
            double waitTime = targetFrameTime - deltaDrawTime;

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

    return !gameShouldQuit;
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
    postReadySignal.Emit();
}

void duin::Application::EnginePreFrame()
{
    preFrameSignal.Emit();
}

void duin::Application::EngineOnEvent(Event e)
{
    if (duin::Input::IsKeyDown(DN_KEY_ESCAPE))
    {
        DN_CORE_INFO("Quit event called... {}", e.sdlEvent.key.key);
        gameShouldQuit = true;
    }

    rootGameObject->ObjectOnEvent(e);
    postInputSignal.Emit(e);
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
    postUpdateSignal.Emit(delta);
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
    postPhysicsUpdateSignal.Emit(delta);

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
    postDrawSignal.Emit();
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
    postDrawUISignal.Emit();
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
    postDebugSignal.Emit();
}

void duin::Application::EnginePostFrame()
{
    postFrameSignal.Emit();
}

void duin::Application::EngineExit()
{
    exitSignal.Emit();
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
