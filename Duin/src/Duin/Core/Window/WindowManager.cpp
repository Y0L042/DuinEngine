#include "dnpch.h"
#include "WindowManager.h"

duin::WindowManager &duin::WindowManager::Get()
{
    static WindowManager wm;
    return wm;
}

std::shared_ptr<duin::WindowState> duin::WindowManager::CreateNewWindow(int width, int height, const std::string &title)
{
    auto winState = std::make_shared<WindowState>();
    winState->SetSDLWindow(::SDL_CreateWindow(title.c_str(), width, height, 0));
    if (!winState->IsWindowValid())
    {
        DN_CORE_ERROR("Window could not be created! SDL error: {}", ::SDL_GetError());
        return winState;
    }
    winState->SetSDLSurface(::SDL_GetWindowSurface(winState->GetSDLWindow()));
    Get().sdlWindows[winState->GetSDLWindow()] = winState;
    return winState;
}