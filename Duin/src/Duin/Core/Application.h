/**
 * @file Application.h
 * @brief Main application class and engine utility functions.
 * @ingroup Core_Application
 *
 * Provides the Application base class which manages the engine lifecycle,
 * along with global utility functions for window management, timing, and
 * callback registration.
 */

#pragma once

#include <string>
#include <memory>
#include <functional>

#include "Duin/Core/Core.h"
#include "Duin/Core/Events/Event.h"
#include "Duin/Objects/GameObject.h"
#include "Duin/Core/Signals/SignalsModule.h"

namespace duin
{

/** @brief Returns the root directory path of the application. */
std::string GetRootDirectory();
/** @brief Pauses game updates (debug feature). */
void DebugPauseGame();
/** @brief Resumes game updates after pause. */
void DebugResumeGame();
/** @brief Checks if game is paused. @return Non-zero if paused. */
int DebugIsGamePaused();

/** @brief Sets the target framerate. @param framerate Target frames per second. */
void SetFramerate(int framerate);
/** @brief Draws physics FPS at screen position. */
void DrawPhysicsFPS(float x, float y);
/** @brief Draws render FPS at screen position. */
void DrawRenderFPS(float x, float y);
/** @brief Returns current physics update rate. */
float GetPhysicsFPS();
/** @brief Returns time spent on last physics frame in seconds. */
float GetPhysicsFrameTime();
/** @brief Returns current render framerate. */
float GetRenderFPS();
/** @brief Returns time spent on last render frame in seconds. */
float GetRenderFrameTime();
/** @brief Returns total physics frames since start. */
size_t GetPhysicsFrameCount();
/** @brief Returns total render frames since start. */
size_t GetRenderFrameCount();
/** @brief Returns elapsed time in seconds since SDL init. */
double GetTicks();
/** @brief Returns elapsed time in milliseconds since SDL init. */
double GetTicksMilli();
/** @brief Returns elapsed time in nanoseconds since SDL init. */
double GetTicksNano();
/** @brief Delays execution for specified seconds. */
void DelayProcess(float seconds);
/** @brief Delays execution for specified milliseconds. */
void DelayProcessMilli(float milliseconds);
/** @brief Returns window width in pixels. */
int GetWindowWidth();
/** @brief Returns window height in pixels. */
int GetWindowHeight();

/** @brief Enables or disables window resizing. */
void SetWindowResizable(bool enable);
/** @brief Maximizes the application window. */
void MaximizeWindow();
/** @brief Minimizes the application window. */
void MinimizeWindow();
/** @brief Sets whether to pause updates when window is minimized. */
void SetPauseOnMinimized(bool enable);
/** @brief Enables or disables ImGui docking in main viewport. */
void SetAllowDockingInMain(bool enable);
/** @brief Sets the path for ImGui .ini settings file. */
void SetImGuiINIPath(const std::string &newPath);

/** @brief Returns the underlying SDL window handle. */
::SDL_Window *GetSDLWindow();

/**
 * @name Post-Callback Registration
 * Register callbacks to be invoked after engine lifecycle stages.
 * @{
 */
void QueuePostReadyCallback(std::function<void(void)> f);
void QueuePostInputCallback(std::function<void(Event)> f);
void QueuePostUpdateCallback(std::function<void(double)>);
void QueuePostPhysicsUpdateCallback(std::function<void(double)>);
void QueuePostDrawCallback(std::function<void()>);
void QueuePostDrawUICallback(std::function<void()>);
void QueuePreFrameCallback(std::function<void()>);
void QueuePostFrameCallback(std::function<void()>);
void QueuePostDebugCallback(std::function<void()>);
/** @} */

/** @brief Registers a callback to be invoked on application exit. */
void QueueExitCallback(std::function<void()>);

/**
 * @class Application
 * @brief Base class for Duin applications.
 * @ingroup Core_Application
 *
 * Application manages the engine's main loop, window creation, and subsystem
 * coordination. User applications inherit from this class and override virtual
 * methods to implement game logic.
 *
 * Lifecycle order:
 * 1. Initialize() - one-time setup
 * 2. Ready() - called after all systems initialized
 * 3. Main loop:
 *    - OnEvent() - input events
 *    - Update() - per-frame logic (variable timestep)
 *    - PhysicsUpdate() - fixed timestep (60 Hz default)
 *    - Draw() - 3D rendering
 *    - DrawUI() - ImGui rendering
 * 4. Exit() - cleanup
 *
 * @code
 * class MyGame : public duin::Application {
 * public:
 *     void Initialize() override {
 *         SetWindowName("My Game");
 *         SetWindowStartupSize(1280, 720);
 *     }
 *     void Update(double delta) override {
 *         // Game logic
 *     }
 * };
 *
 * duin::Application* duin::CreateApplication() {
 *     return new MyGame();
 * }
 * @endcode
 */
class DAPI Application
{
  public:
    Application();
    virtual ~Application();

    /** @brief Sets initial window dimensions before Run(). */
    void SetWindowStartupSize(int width, int height);
    /** @brief Sets the window title. */
    void SetWindowName(const char *string);

    /** @brief Starts the main loop. Call once after configuration. */
    void Run();

    /** @brief Internal engine initialization. Do not call directly. */
    void EngineInitialize();
    /** @brief Override to perform one-time setup. Called before Ready(). */
    virtual void Initialize();

    void EngineReady();
    /** @brief Override to perform setup after all systems are initialized. */
    virtual void Ready();
    void EnginePostReady();

    void EnginePreFrame();

    void EngineOnEvent(Event event);
    /** @brief Override to handle input events. @param event The input event. */
    virtual void OnEvent(Event event);

    void EngineUpdate(double delta);
    /** @brief Override for per-frame logic. @param delta Time since last frame in seconds. */
    virtual void Update(double delta);
    void EnginePostUpdate(double delta);

    void EnginePhysicsUpdate(double delta);
    /** @brief Override for fixed-timestep physics logic. @param delta Fixed timestep in seconds. */
    virtual void PhysicsUpdate(double delta);
    void EnginePostPhysicsUpdate(double delta);

    void EngineDraw();
    /** @brief Override for 3D rendering. */
    virtual void Draw();
    void EnginePostDraw();

    void EngineDrawUI();
    /** @brief Override for ImGui UI rendering. */
    virtual void DrawUI();
    void EnginePostDrawUI();

    void EngineDebug();
    /** @brief Override for debug rendering and logic. */
    virtual void Debug();
    void EnginePostDebug();

    void EnginePostFrame();

    /** @brief Override for cleanup before shutdown. */
    virtual void Exit();
    void EngineExit();

    /**
     * @brief Creates a child GameObject attached to the root.
     * @tparam T GameObject-derived type to create.
     * @tparam Args Constructor argument types.
     * @param args Arguments forwarded to T's constructor.
     * @return Shared pointer to the created object.
     */
    template <typename T, typename... Args>
    std::shared_ptr<T> CreateChildObject(Args... args)
    {
        return rootGameObject->CreateChildObject<T>(std::forward<Args>(args)...);
    }
    /** @brief Adds an existing GameObject as a child of the root. */
    void AddChildObject(std::shared_ptr<GameObject> child);
    /** @brief Removes a child GameObject from the root. */
    void RemoveChildObject(std::shared_ptr<GameObject> child);

  private:
    std::string windowName = "Game";
    std::shared_ptr<GameObject> rootGameObject;
};

/**
 * @brief User-defined factory function for the application.
 *
 * Implement this function to return your Application-derived class instance.
 * The engine calls this at startup.
 *
 * @return Pointer to the application instance.
 */
Application *CreateApplication();

} // namespace duin
