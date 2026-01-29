/**
 * @file Process.h
 * @brief External process management.
 * @ingroup Core
 *
 * Provides functionality to spawn and manage external processes.
 * Wraps SDL3 process API.
 */

#pragma once

#include <SDL3/SDL_process.h>
#include <memory>
#include <string>

namespace duin
{

/**
 * @struct Process
 * @brief Handle to an external process.
 * @ingroup Core
 *
 * Represents a spawned external process. Query state with IsAlive(),
 * and retrieve exit code after termination with GetExitcode().
 */
struct Process
{
    /** @brief Returns true if the process is still running. */
    bool IsAlive();
    /** @brief Returns true if process was spawned in background mode. */
    bool IsBackground();
    /** @brief Returns the exit code (valid only after process terminates). */
    int GetExitcode();

    SDL_Process *sdlProcess;       ///< Underlying SDL process handle.
    bool hasBeenDestroyed = false; ///< True if DestroyProcessObject was called.
};

/**
 * @brief Spawns an external process.
 * @param args Null-terminated array of arguments. args[0] is the executable path.
 * @param pipe_stdio If true, pipes stdout/stderr for capture.
 * @return Shared pointer to Process, or nullptr on failure.
 *
 * @code
 * const char* args[] = { "myprogram", "--flag", nullptr };
 * auto proc = duin::CreateProcess(args, false);
 * @endcode
 */
std::shared_ptr<Process> CreateProcess(const char *const *args, bool pipe_stdio);

/**
 * @brief Releases process resources.
 * @param process The process to clean up.
 *
 * Call after the process has terminated and you no longer need the handle.
 */
void DestroyProcessObject(std::shared_ptr<Process> process);

/**
 * @brief Terminates a running process.
 * @param process The process to kill.
 * @param force If true, forcefully terminate (SIGKILL on Unix).
 * @return True if the signal was sent.
 */
bool KillProcess(std::shared_ptr<Process> process, bool force);

} // namespace duin
