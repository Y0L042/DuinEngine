#include "dnpch.h"
#include "Application.h"

#include <GLFW/glfw3.h>
#include <imgui_impl_glfw.h>
#include <imgui_impl_opengl3.h>

#include <filesystem>

#define RAYMATH_IMPLEMENTATION
#define RCAMERA_IMPLEMENTATION
#define RLIGHTS_IMPLEMENTATION

static int debugIsGamePaused_ = 0;

static size_t physicsFrameCount = 0;
static size_t renderFrameCount = 0;

static double physicsFrameTime = 0.0;
static double renderFrameTime = 0.0;

static int TARGET_RENDER_FRAMERATE = 72;
static int TARGET_PHYSICS_FRAMERATE = 60;

static int screenWidth = 1280;
static int screenHeight = 720;

Color backgroundColor = WHITE;
::Camera3D activeCamera3D;

namespace duin {

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

void SetActiveCamera3D(::Camera3D camera3d)
{
    activeCamera3D = camera3d;
}

void SetBackgroundColor(::Color color)
{
    backgroundColor = color;
}

void SetFramerate(int framerate)
{
    TARGET_RENDER_FRAMERATE = framerate;
}

/*
void DrawPhysicsFPS(float x, float y)
{
    char buffer[10];
    std::snprintf(buffer, sizeof(buffer), "P: %.2f", GetPhysicsFPS());
    ::DrawText(buffer, x, y, 30, ::GREEN);
}

void DrawRenderFPS(float x, float y)
{
    char buffer[10];
    std::snprintf(buffer, sizeof(buffer), "R: %.2f", GetRenderFPS());
    ::DrawText(buffer, x, y, 30, ::GREEN);
}
*/
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
    ::DrawText(buffer, x, y, 30, ::GREEN);
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
    ::DrawText(buffer, x, y, 30, ::GREEN);
}

float GetPhysicsFPS()
{
    float fps = 1.0f / (float)physicsFrameTime;
    return fps;
}

float GetPhysicsFrameTime()
{
    float frametime = physicsFrameTime;
    return frametime;
}

float GetRenderFPS()
{
    float fps = 1.0f / (float)renderFrameTime;
    return fps;
}

float GetRenderFrameTime()
{
    float frametime = renderFrameTime;
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
    const double MAX_ACCUMULATOR = 0.25;
    double physicsCurrentTime = ::GetTime();
    double physicsPreviousTime = 0.0;
    double physicsDeltaTime = 0.0;
    double physicsAccumTime = 0.0;

    double previousTime = ::GetTime();
    double currentTime = 0.0;
    double updateDrawTime = 0.0;
    double waitTime = 0.0;
    double deltaTime = 0.0;

    EngineInitialize();
    Initialize();

    DN_CORE_INFO("Set render FPS {}", TARGET_RENDER_FRAMERATE);

    ::SetConfigFlags(
              FLAG_MSAA_4X_HINT 
            | FLAG_WINDOW_RESIZABLE 
            | FLAG_WINDOW_HIGHDPI
            // | FLAG_VSYNC_HINT 
    );

    ::InitWindow(screenWidth, screenHeight, windowName.c_str());
    ::rlImGuiSetup(true);

    EngineReady();
    Ready();

    while(!::WindowShouldClose()) {
        #ifdef DN_DEBUG
            if (!debugIsGamePaused_) {
        #endif /* DN_DEBUG */

        EnginePreFrame();

        PollInputEvents();
        EngineHandleInputs();
        HandleInputs();

        EngineUpdate(deltaTime);
        Update(deltaTime);
        EnginePostUpdate(deltaTime);

        physicsCurrentTime = ::GetTime();
        physicsDeltaTime = physicsCurrentTime - physicsPreviousTime;
        physicsAccumTime += physicsDeltaTime;
        static const double physicsTimeStep = (1.0 / (double)TARGET_PHYSICS_FRAMERATE);
        while (physicsAccumTime >= physicsTimeStep) {
            physicsPreviousTime = physicsCurrentTime;
            physicsAccumTime -= physicsTimeStep;

            physicsFrameTime = physicsTimeStep;
            ++physicsFrameCount;

            EnginePhysicsUpdate(physicsDeltaTime);
            PhysicsUpdate(physicsDeltaTime); 
            EnginePostPhysicsUpdate(physicsDeltaTime);
        } // End of Physics

        ::BeginDrawing();
        ::rlImGuiBegin();

        ::ClearBackground(::Color{ 
                backgroundColor.r, 
                backgroundColor.g, 
                backgroundColor.b, 
                backgroundColor.a });

        ImGui::DockSpaceOverViewport(0,  NULL, ImGuiDockNodeFlags_PassthruCentralNode);

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
        ::SwapScreenBuffer();
        EnginePostFrame();

        renderFrameTime = updateDrawTime;
        ++renderFrameCount;
        static const double targetFrameTime = 1.0 / (double)TARGET_RENDER_FRAMERATE;
        currentTime = ::GetTime();
        updateDrawTime = currentTime - previousTime;
        if (TARGET_RENDER_FRAMERATE > -1) {
            waitTime = targetFrameTime - updateDrawTime;
            if (waitTime > 0.0) {
                ::WaitTime((float)waitTime);
                currentTime = ::GetTime();
                deltaTime = currentTime - previousTime;
            }
        } else { deltaTime = updateDrawTime; }
        previousTime = currentTime;

        #ifdef DN_DEBUG
            } 
            EngineDebug();
            Debug();
            EnginePostDebug();
        #endif /* DN_DEBUG */

    } // End of Application While

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
