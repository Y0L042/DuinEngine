#include "dnpch.h"
#include "Process.h"

std::shared_ptr<duin::Process> duin::CreateProcess(const char *const *args, bool pipe_stdio)
{
    std::shared_ptr<Process> proc = std::make_shared<Process>();
    proc->sdlProcess = SDL_CreateProcess(args, pipe_stdio);

    return proc;
}

void duin::DestroyProcessObject(std::shared_ptr<Process> process)
{
    process->hasBeenDestroyed = true;
    SDL_DestroyProcess(process->sdlProcess);
}

bool duin::KillProcess(std::shared_ptr<Process> process, bool force)
{
    bool res = SDL_KillProcess(process->sdlProcess, force);

    return res;
}

bool duin::Process::IsAlive()
{
    return false;
}

bool duin::Process::IsBackground()
{
    return false;
}

int duin::Process::GetExitcode()
{
    return 0;
}
