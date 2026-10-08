#include "dnpch.h"
#include "WindowCtx.h"

bool duin::WindowCtx::IsWindowValid()
{
    return sdlWindow != nullptr && ::SDL_GetWindowID(sdlWindow) != 0;
}

void duin::WindowCtx::SetSize(int width, int height)
{
    if (!IsWindowValid())
    {
        DN_CORE_WARN("WindowCtx::SetSize called on invalid window.");
        return;
    }
    if (!::SDL_SetWindowSize(sdlWindow, width, height))
    {
        DN_CORE_WARN("SDL_SetWindowSize failed: {}", ::SDL_GetError());
    }
}

void duin::WindowCtx::GetSize(int &width, int &height)
{
    width = 0;
    height = 0;
    if (!IsWindowValid())
    {
        return;
    }
    if (!::SDL_GetWindowSize(sdlWindow, &width, &height))
    {
        DN_CORE_WARN("SDL_GetWindowSize failed: {}", ::SDL_GetError());
    }
}

int duin::WindowCtx::GetWidth()
{
    int width, height;
    GetSize(width, height);
    return width;
}

int duin::WindowCtx::GetHeight()
{
    int width, height;
    GetSize(width, height);
    return height;
}

void duin::WindowCtx::SetTitle(const std::string &title)
{
    if (!IsWindowValid())
    {
        DN_CORE_WARN("WindowCtx::SetTitle called on invalid window.");
        return;
    }
    if (!::SDL_SetWindowTitle(sdlWindow, title.c_str()))
    {
        DN_CORE_WARN("SDL_SetWindowTitle failed: {}", ::SDL_GetError());
    }
}

std::string duin::WindowCtx::GetTitle()
{
    if (!IsWindowValid())
    {
        return std::string();
    }
    return std::string(::SDL_GetWindowTitle(sdlWindow));
}

void duin::WindowCtx::UseCustomImguiPath(bool customImguiPath)
{
    this->customImguiPath = customImguiPath;
}

bool duin::WindowCtx::UsingCustomImguiPath()
{
    return customImguiPath;
}

void duin::WindowCtx::SetImguiPath(const std::string &imguiPath)
{
    this->imguiPath = imguiPath;
}

const std::string &duin::WindowCtx::GetImguiPath()
{
    return imguiPath;
}

void duin::WindowCtx::SetPauseOnMinimize(bool pauseOnMinimize)
{
    this->pauseOnMinimize = pauseOnMinimize;
}

bool duin::WindowCtx::GetPauseOnMinimize()
{
    return pauseOnMinimize;
}

void duin::WindowCtx::SetAllowDockingInMain(bool allowDockingInMain)
{
    this->allowDockingInMain = allowDockingInMain;
}

bool duin::WindowCtx::GetAllowDockingInMain()
{
    return allowDockingInMain;
}

void duin::WindowCtx::SetSDLWindow(SDL_Window *sdlWindow)
{
    this->sdlWindow = sdlWindow;
}

SDL_Window *duin::WindowCtx::GetSDLWindow()
{
    return sdlWindow;
}

void duin::WindowCtx::SetSDLSurface(SDL_Surface *sdlSurface)
{
    this->sdlSurface = sdlSurface;
}

SDL_Surface *duin::WindowCtx::GetSDLSurface()
{
    return sdlSurface;
}

void duin::WindowCtx::SetSDLWindowFlags(SDL_WindowFlags sdlWindowFlags)
{
    if (!IsWindowValid())
    {
        DN_CORE_WARN("WindowCtx::SetSDLWindowFlags called on invalid window.");
        return;
    }

    const SDL_WindowFlags changed = ::SDL_GetWindowFlags(sdlWindow) ^ sdlWindowFlags;
    auto has = [&](SDL_WindowFlags flag) { return (sdlWindowFlags & flag) != 0; };

    if (changed & SDL_WINDOW_FULLSCREEN)
        ::SDL_SetWindowFullscreen(sdlWindow, has(SDL_WINDOW_FULLSCREEN));
    if (changed & SDL_WINDOW_BORDERLESS)
        ::SDL_SetWindowBordered(sdlWindow, !has(SDL_WINDOW_BORDERLESS));
    if (changed & SDL_WINDOW_RESIZABLE)
        ::SDL_SetWindowResizable(sdlWindow, has(SDL_WINDOW_RESIZABLE));
    if (changed & SDL_WINDOW_ALWAYS_ON_TOP)
        ::SDL_SetWindowAlwaysOnTop(sdlWindow, has(SDL_WINDOW_ALWAYS_ON_TOP));
    if (changed & SDL_WINDOW_MODAL)
        ::SDL_SetWindowModal(sdlWindow, has(SDL_WINDOW_MODAL));
    if (changed & SDL_WINDOW_NOT_FOCUSABLE)
        ::SDL_SetWindowFocusable(sdlWindow, !has(SDL_WINDOW_NOT_FOCUSABLE));
    if (changed & SDL_WINDOW_MOUSE_GRABBED)
        ::SDL_SetWindowMouseGrab(sdlWindow, has(SDL_WINDOW_MOUSE_GRABBED));
    if (changed & SDL_WINDOW_KEYBOARD_GRABBED)
        ::SDL_SetWindowKeyboardGrab(sdlWindow, has(SDL_WINDOW_KEYBOARD_GRABBED));
    if (changed & SDL_WINDOW_MOUSE_RELATIVE_MODE)
        ::SDL_SetWindowRelativeMouseMode(sdlWindow, has(SDL_WINDOW_MOUSE_RELATIVE_MODE));
    if (changed & SDL_WINDOW_HIDDEN)
        has(SDL_WINDOW_HIDDEN) ? ::SDL_HideWindow(sdlWindow) : ::SDL_ShowWindow(sdlWindow);
    if (changed & (SDL_WINDOW_MINIMIZED | SDL_WINDOW_MAXIMIZED))
    {
        if (has(SDL_WINDOW_MINIMIZED))
            ::SDL_MinimizeWindow(sdlWindow);
        else if (has(SDL_WINDOW_MAXIMIZED))
            ::SDL_MaximizeWindow(sdlWindow);
        else
            ::SDL_RestoreWindow(sdlWindow);
    }
}

SDL_WindowFlags duin::WindowCtx::GetSDLWindowFlags()
{
    if (!IsWindowValid())
    {
        return 0;
    }
    return ::SDL_GetWindowFlags(sdlWindow);
}

void duin::WindowCtx::SetTargetRenderFramerate(int targetRenderFramerate)
{
    this->targetRenderFramerate = targetRenderFramerate;
}

int duin::WindowCtx::GetTargetRenderFramerate()
{
    return targetRenderFramerate;
}

void duin::WindowCtx::ProcessWindowEvent(WindowEvent event)
{
    DN_CORE_INFO("Window Event!");
    if (event.sdlEvent.type == SDL_EventType::SDL_EVENT_WINDOW_CLOSE_REQUESTED)
    {
        ShutDown();
    }
}

void duin::WindowCtx::ShutDown()
{
    DN_CORE_INFO("Shutting down window...");
    ::SDL_DestroySurface(sdlSurface);
    sdlSurface = nullptr;
    ::SDL_DestroyWindow(sdlWindow);
    sdlWindow = nullptr;
}
