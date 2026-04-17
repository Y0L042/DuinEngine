#include "dnpch.h"
#include "Application.h"

#include "Duin/Core/Events/EventHandler.h"
#include "Duin/Core/Events/ScanCodes.h"
#include "Duin/Core/Events/Keys.h"
#include "Duin/Core/Events/EngineInput.h"
#include "Duin/Core/Events/Input.h"
#include "Duin/Render/Renderer.h"

#define SDL_MAIN_HANDLED

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
#include <Duin/Physics/PhysicsIncludes.h>
#include <Duin/Render/Camera.h>
#include <external/imgui.h>
#include <SDL3/SDL_error.h>
#include <SDL3/SDL_events.h>
#include <SDL3/SDL_init.h>
#include <SDL3/SDL_log.h>
#include <SDL3/SDL_properties.h>
#include <SDL3/SDL_stdinc.h>
#include <SDL3/SDL_surface.h>
#include <SDL3/SDL_timer.h>
#include <SDL3/SDL_video.h>

// ---------------------------------------------------------------------------
// Debug / quit state
// ---------------------------------------------------------------------------
static int debugIsGamePaused_ = 0;
static bool isPhysicsPaused = false;
static bool isUpdatePaused = false;
static bool gameShouldQuit = false;

// ---------------------------------------------------------------------------
// Frame timing
// ---------------------------------------------------------------------------
static size_t physicsFrameCount = 0;
static size_t renderFrameCount = 0;
static double physicsFrameTime = 0.0;
static double renderFrameTime = 0.0;
static int TARGET_RENDER_FRAMERATE = 60;
static int TARGET_PHYSICS_FRAMERATE = 60;

// ---------------------------------------------------------------------------
// Window state
// ---------------------------------------------------------------------------
static int WINDOW_WIDTH = 1280;
static int WINDOW_HEIGHT = 720;
static bool USE_CUSTOM_IMGUI_INI_PATH = false;
static std::string IMGUI_INI_PATH = "./";
static bool PAUSE_ON_MINIMIZE = false;
static bool ALLOW_DOCKING_IN_MAIN = false;
static SDL_Window *sdlWindow = NULL;
static SDL_Surface *sdlSurface = NULL;
static SDL_WindowFlags sdlWindowFlags = 0;

// ---------------------------------------------------------------------------
// Lifecycle signals
// ---------------------------------------------------------------------------
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

// --- Utility / Accessors ---

std::string duin::GetRootDirectory()
{
    std::filesystem::path currentPath = std::filesystem::current_path();
    return currentPath.string();
}

// --- Debug ---

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

void duin::PausePhysics()
{
    isPhysicsPaused = true;
}

void duin::ResumePhysics()
{
    isPhysicsPaused = false;
}

bool duin::IsPhysicsPaused()
{
    return isPhysicsPaused;
}

void duin::PauseUpdate()
{
    isUpdatePaused = true;
}

void duin::ResumeUpdate()
{
    isUpdatePaused = false;
}

bool duin::IsUpdatePaused()
{
    return isUpdatePaused;
}

// --- Timing ---

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

// --- Window ---

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

SDL_Window *duin::GetSDLWindow()
{
    return sdlWindow;
}

// --- ImGui ---

void duin::SetImGuiINIPath(const std::string &newPath)
{
    DN_CORE_INFO("Custom ImGui INI path set: {}", newPath.c_str());
    USE_CUSTOM_IMGUI_INI_PATH = true;
    IMGUI_INI_PATH = newPath;
    ImGui::GetIO().IniFilename = NULL;
    ImGui::LoadIniSettingsFromDisk(IMGUI_INI_PATH.c_str());
}

// --- Callback Registration ---

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

// --- Application Lifecycle ---

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

// --- Initialization (shared / testing) ---

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

// --- Frame loop (shared) ---

void duin::Application::RunUpdate(double delta)
{
    if (isUpdatePaused)
        return; // TODO Debugging, refactor
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

        if (isPhysicsPaused)
            return; // TODO Debugging, refactor
        EnginePhysicsUpdate(physicsDeltaTime);
        PhysicsUpdate(physicsDeltaTime);
        EnginePostPhysicsUpdate(physicsDeltaTime);
    } // End of Physics
}

// --- Engine Callbacks ---

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
    if (duin::Input::IsKeyDown(DN_SCANCODE_ESCAPE))
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

// --- Object Management ---

void duin::Application::AddChildObject(std::shared_ptr<GameObject> child)
{
    rootGameObject->AddChildObject(child);
}

void duin::Application::RemoveChildObject(std::shared_ptr<GameObject> child)
{
    rootGameObject->RemoveChildObject(child);
}

// ============================================================
// Standard function definitions
// ============================================================

#include "Duin/Render/RHI.h"

#ifndef DN_HEADLESS
#include "Application_Impl.hpp"
#else /* DN_HEADLESS */
#include "Application_HeadlessImpl.hpp"
#endif /* DN_HEADLESS */
