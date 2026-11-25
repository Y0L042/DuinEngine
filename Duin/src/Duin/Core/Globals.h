#pragma once

#include <cstddef>
#include <cstdint>
#include <string>
#include <memory>
#include <functional>
#include <vector>

struct SDL_Window;

namespace duin
{
class WindowCtx;
class EcsPipeline;
class Event;
class World;
class ScopedConnection;

/** @brief Returns the root directory path of the application. */
std::string GetRootDirectory();
/** @brief Pauses game updates (debug feature). */
void DebugPauseGame();
/** @brief Resumes game updates after pause. */
void DebugResumeGame();
/** @brief Checks if game is paused. @return Non-zero if paused. */
int DebugIsGamePaused();
void PausePhysics();
void ResumePhysics();
bool IsPhysicsPaused();
void PauseUpdate();
void ResumeUpdate();
bool IsUpdatePaused();

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

std::shared_ptr<WindowCtx> CreateNewWindow(const std::string &title, int width, int height, uint64_t flags = 0);
/** @brief Requests destruction of a secondary window. Returns false for the primary window or unknown windows. */
bool DestroyWindow(const std::shared_ptr<WindowCtx> &window);
/** @brief Returns all open secondary (non-primary) windows. */
std::vector<std::shared_ptr<WindowCtx>> GetSecondaryWindows();

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

bool GetGameShouldQuit();
void SetGameShouldQuit(bool shouldQuit = true);

std::shared_ptr<EcsPipeline> CreateEcsPipeline(World world);

/**
 * @name Post-Callback Registration
 * Register callbacks to be invoked after engine lifecycle stages.
 * Returns a shared_ptr<ScopedConnection> — keep it alive to keep the callback connected.
 * @{
 */
std::shared_ptr<ScopedConnection> QueuePostReadyCallback(std::function<void(void)> f);
std::shared_ptr<ScopedConnection> QueuePostInputCallback(std::function<void(Event)> f);
std::shared_ptr<ScopedConnection> QueuePostUpdateCallback(std::function<void(double)>);
std::shared_ptr<ScopedConnection> QueuePostPhysicsUpdateCallback(std::function<void(double)>);
std::shared_ptr<ScopedConnection> QueuePostDrawCallback(std::function<void()>);
std::shared_ptr<ScopedConnection> QueuePostDrawUICallback(std::function<void()>);
std::shared_ptr<ScopedConnection> QueuePreFrameCallback(std::function<void()>);
std::shared_ptr<ScopedConnection> QueuePostFrameCallback(std::function<void()>);
std::shared_ptr<ScopedConnection> QueuePostDebugCallback(std::function<void()>);
/** @} */

/** @brief Registers a callback to be invoked on application exit. */
std::shared_ptr<ScopedConnection> QueueExitCallback(std::function<void()>);
} // namespace duin