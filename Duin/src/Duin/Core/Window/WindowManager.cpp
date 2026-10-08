#include "dnpch.h"
#include "WindowManager.h"
#include <Duin/Core/Events/EventHandler.h>
#include <Duin/Core/Events/Event.h>

duin::WindowManager &duin::WindowManager::Get()
{
    static WindowManager wm;
    return wm;
}

std::shared_ptr<duin::WindowCtx> duin::WindowManager::CreateNewWindow(const std::string &title, int width, int height, uint64_t flags)
{
    auto winState = std::make_shared<WindowCtx>();
    winState->SetSDLWindow(::SDL_CreateWindow(title.c_str(), width, height, flags));
    if (!winState->IsWindowValid())
    {
        DN_CORE_ERROR("Window could not be created! SDL error: {}", ::SDL_GetError());
        return winState;
    }
    winState->SetSDLSurface(::SDL_GetWindowSurface(winState->GetSDLWindow()));
    if (Get().primaryWindow == nullptr)
    {
        Get().primaryWindow = winState;
    }
    Get().sdlWindows[winState->GetSDLWindow()] = winState;
    return winState;
}

void duin::WindowManager::SubscribeToWindowEvents()
{
    EventHandler::Get().RegisterWindowEventListener(
        std::function<void(WindowEvent)>([](WindowEvent event) { WindowManager::Get().OnWindowEvent(event); }));
}

void duin::WindowManager::SetPrimaryWindow(std::shared_ptr<WindowCtx> windowCtx)
{
    primaryWindow = windowCtx;
}

void duin::WindowManager::OnWindowEvent(WindowEvent event)
{
    SDL_Window *window = ::SDL_GetWindowFromID(event.sdlEvent.window.windowID);
    std::shared_ptr<WindowCtx> state = sdlWindows[window];
    if (state != nullptr)
    {
        // Process event
        state->ProcessWindowEvent(event);
    }
    else
    {
        DN_CORE_FATAL("Event's Window not found in WindowManager!");
    }
}
