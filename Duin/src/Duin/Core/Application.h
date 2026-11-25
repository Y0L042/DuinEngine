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
#include "Duin/Core/Events/EventHandler.h"
#include "Duin/Core/Window/WindowManager.h"
#include "Duin/Objects/GameObject.h"
#include "Duin/Core/Signals/SignalsModule.h"
#include "Duin/ECS/DECS/World.h"

#include "DEBUG_DEFINES.h"

namespace duin
{
class WindowCtx;
class EcsPipeline;

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
 * duin::Application* duin::CreateApplication(int argc, char **argv) {
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

    EventHandler& GetEventManager()
    {
        return eventHandler;
    }

    WindowManager& GetWindowManager()
    {
        return windowManager;
    }

    /** @brief Starts the main loop. Call once after configuration. */
    void Run();

#ifdef DN_HEADLESS
    /** @brief Enables headless mode (no window/ImGui). Call before Run() or InitSDL(). */
    void SetHeadless(bool headless);
#endif /* DN_HEADLESS */

#ifdef DN_TESTING
    void InitSDL();
    void InitRHI();
    void InitImGui();
    void InitRenderer();
    void CleanRenderer();
    void ShutdownImGui();
    void ShutdownRHI();
    void ShutdownSDL();

    void BeginRenderFrame();
    void EndRenderFrame();

#ifdef DN_HEADLESS
    bool ProcessFrame(
        double &deltaTime, double &physicsCurrentTime, double &physicsPreviousTime, double &physicsAccumTime);
    bool PushSDLEvent(::SDL_Event *e);
#else
    bool ProcessFrame(
        double &deltaTime, double &physicsCurrentTime, double &physicsPreviousTime, double &physicsAccumTime);
#endif /* DN_HEADLESS */
#endif /* DN_TESTING */

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
#ifdef DN_HEADLESS
    bool headlessMode = false;
#endif /* DN_HEADLESS */

    std::string windowName = "Game";
    std::shared_ptr<GameObject> rootGameObject;
    EventHandler eventHandler;
    WindowManager windowManager;

#ifndef DN_TESTING
    // Init / shutdown
    void InitSDL();
    void InitRHI();
    void InitImGui();
    void ShutdownImGui();
    void ShutdownRHI();
    void ShutdownSDL();

    bool ProcessFrame(
        double &deltaTime, double &physicsCurrentTime, double &physicsPreviousTime, double &physicsAccumTime);
#endif

    // Per-frame
    void ProcessEvents();
    void RunUpdate(double delta);
    void RunPhysics(double &physicsCurrentTime, double &physicsPreviousTime, double &physicsAccumTime);
    void RunRender();

    void InitWindowRenderResources(WindowCtx &windowCtx);
    void ShutdownWindowRenderResources(WindowCtx &windowCtx);
    void RenderSecondaryWindow(WindowCtx &windowCtx);

    void PhysicsStep(double frametime);
};

/**
 * @brief User-defined factory function for the application.
 *
 * Implement this function to return your Application-derived class instance.
 * The engine calls this at startup.
 *
 * @return Pointer to the application instance.
 */
Application *CreateApplication(int argc, char **argv);

Application *GetApplication();

} // namespace duin
