#include "dnpch.h"
#include "WindowManager.h"
#include <Duin/Core/Events/EventHandler.h>
#include <Duin/Core/Events/Event.h>

void duin::WindowManager::SetEventHandler(EventHandler *eventHandler)
{
    this->eventHandler = eventHandler;
    if (eventHandler != nullptr)
    {
        SubscribeToWindowEvents();
    }
}

std::shared_ptr<duin::WindowCtx> duin::WindowManager::CreateNewWindow(
    const std::string &title, int width, int height, uint64_t flags)
{
    std::shared_ptr<WindowCtx> windowCtx = std::make_shared<WindowCtx>();
    windowCtx->SetSDLWindow(::SDL_CreateWindow(title.c_str(), width, height, flags));
    if (!windowCtx->IsWindowValid())
    {
        DN_CORE_FATAL("Window could not be created! SDL error: {}", ::SDL_GetError());
        return nullptr;
    }
    if (primaryWindow == nullptr)
    {
        primaryWindow = windowCtx;
    }
    sdlWindows[windowCtx->GetSDLWindow()] = windowCtx;
    OnWindowCreated.Emit(windowCtx);
    return windowCtx;
}

bool duin::WindowManager::DestroyWindow(SDL_Window *window)
{
    if (primaryWindow && (primaryWindow->GetSDLWindow() == window))
    {
        primaryWindow = nullptr;
        OnPrimaryWindowClosing.Emit();
        // TODO implement proper closure of app
        DN_CORE_DEBUG("Destroying primary Window");
        return true; // Application will quit, destroying the primary window
    }
    else if (sdlWindows.contains(window))
    {
        DN_CORE_DEBUG("WindowManager destroying Window...");
        std::shared_ptr<WindowCtx> windowCtx = sdlWindows[window];
        windowCtx->SetPendingDestroy(true);
        pendingRelease.push_back(windowCtx);
        sdlWindows.erase(window);
        return true;
    }
    return false;
}

std::shared_ptr<duin::WindowCtx> duin::WindowManager::GetWindowCtx(SDL_Window *window)
{
    auto it = sdlWindows.find(window);
    return it != sdlWindows.end() ? it->second : nullptr;
}

std::vector<std::shared_ptr<duin::WindowCtx>> duin::WindowManager::GetSecondaryWindows()
{
    std::vector<std::shared_ptr<WindowCtx>> windows;
    windows.reserve(sdlWindows.size());
    for (auto &[window, windowCtx] : sdlWindows)
    {
        if (windowCtx != primaryWindow)
        {
            windows.push_back(windowCtx);
        }
    }
    return windows;
}

void duin::WindowManager::ReleasePendingWindows()
{
    std::vector<std::shared_ptr<WindowCtx>> released = std::move(pendingRelease);
    pendingRelease.clear();
    for (std::shared_ptr<WindowCtx> &windowCtx : released)
    {
        OnWindowDestroying.Emit(windowCtx);
        pendingDestroy.push_back(windowCtx);
    }
}

void duin::WindowManager::DestroyPendingWindows()
{
    for (std::shared_ptr<WindowCtx> &windowCtx : pendingDestroy)
    {
        windowCtx->Destroy();
    }
    pendingDestroy.clear();
}

void duin::WindowManager::DestroyAllWindows()
{
    for (std::shared_ptr<WindowCtx> &windowCtx : pendingRelease)
    {
        windowCtx->Destroy();
    }
    pendingRelease.clear();
    DestroyPendingWindows();
    for (auto &[window, windowCtx] : sdlWindows)
    {
        windowCtx->Destroy();
    }
    sdlWindows.clear();
    primaryWindow = nullptr;
}

void duin::WindowManager::SubscribeToWindowEvents()
{
    if (eventHandler != nullptr)
    {
        eventHandler->RegisterWindowEventListener(
            std::function<void(WindowEvent)>([&](WindowEvent event) { OnWindowEvent(event); }));
    }
    else
    {
        DN_CORE_FATAL("Failed to set EventHandler reference!");
    }
}

void duin::WindowManager::SetPrimaryWindow(std::shared_ptr<WindowCtx> windowCtx)
{
    primaryWindow = windowCtx;
}

void duin::WindowManager::OnWindowEvent(WindowEvent event)
{
    std::shared_ptr<WindowCtx> windowCtx = nullptr;
    SDL_Window *window = ::SDL_GetWindowFromID(event.sdlEvent.window.windowID);

    if (sdlWindows.contains(window))
    {
        windowCtx = sdlWindows[window];
    }

    if (windowCtx != nullptr)
    {
        if (event.sdlEvent.type == SDL_EventType::SDL_EVENT_WINDOW_CLOSE_REQUESTED)
        {
            DestroyWindow(window);
            return;
        }

        // Process event
        DN_CORE_DEBUG("WindowManager processing WindowEvent.");
        windowCtx->ProcessWindowEvent(event);
    }
    else
    {
        DN_CORE_FATAL("Event's Window not found in WindowManager!");
    }
}
