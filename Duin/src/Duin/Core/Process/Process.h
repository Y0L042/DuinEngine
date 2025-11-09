#pragma once

#include <SDL3/SDL_process.h>
#include <memory>
#include <string>

namespace duin
{
struct Process
{
    bool IsAlive();
    bool IsBackground();
    int GetExitcode();
    SDL_Process *sdlProcess;
    bool hasBeenDestroyed = false;
    // Properties
    // Data
};

/*
The path to the executable is supplied in args[0]. args[1..N] are additional arguments passed on the command line of the
new process, and the argument list should be terminated with a NULL, e.g.:

const char *args[] = { "myprogram", "argument", NULL };
*/
std::shared_ptr<Process> CreateProcess(const char *const *args, bool pipe_stdio);
void DestroyProcessObject(std::shared_ptr<Process> process);
bool KillProcess(std::shared_ptr<Process> process, bool force);



} // namespace duin