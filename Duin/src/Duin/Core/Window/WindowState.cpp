#include "dnpch.h"
#include "WindowState.h"
#include <Duin/Core/Events/Event.h>

bool duin::WindowState::IsWindowValid()
{
    return sdlWindow != nullptr && ::SDL_GetWindowID(sdlWindow) != 0;
}

void duin::WindowState::SetSize(int width, int height)
{
    if (!IsWindowValid())
    {
        DN_CORE_WARN("WindowState::SetSize called on invalid window.");
        return;
    }
    if (!::SDL_SetWindowSize(sdlWindow, width, height))
    {
        DN_CORE_WARN("SDL_SetWindowSize failed: {}", ::SDL_GetError());
    }
}

void duin::WindowState::GetSize(int &width, int &height)
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

int duin::WindowState::GetWidth()
{
    int width, height;
    GetSize(width, height);
    return width;
}

int duin::WindowState::GetHeight()
{
    int width, height;
    GetSize(width, height);
    return height;
}

void duin::WindowState::SetTitle(const std::string &title)
{
    if (!IsWindowValid())
    {
        DN_CORE_WARN("WindowState::SetTitle called on invalid window.");
        return;
    }
    if (!::SDL_SetWindowTitle(sdlWindow, title.c_str()))
    {
        DN_CORE_WARN("SDL_SetWindowTitle failed: {}", ::SDL_GetError());
    }
}

std::string duin::WindowState::GetTitle()
{
    if (!IsWindowValid())
    {
        return std::string();
    }
    return std::string(::SDL_GetWindowTitle(sdlWindow));
}

void duin::WindowState::UseCustomImguiPath(bool customImguiPath)
{
    this->customImguiPath = customImguiPath;
}

bool duin::WindowState::UsingCustomImguiPath()
{
    return customImguiPath;
}

void duin::WindowState::SetImguiPath(const std::string &imguiPath)
{
    this->imguiPath = imguiPath;
}

const std::string &duin::WindowState::GetImguiPath()
{
    return imguiPath;
}

void duin::WindowState::SetPauseOnMinimize(bool pauseOnMinimize)
{
    this->pauseOnMinimize = pauseOnMinimize;
}

bool duin::WindowState::GetPauseOnMinimize()
{
    return pauseOnMinimize;
}

void duin::WindowState::SetAllowDockingInMain(bool allowDockingInMain)
{
    this->allowDockingInMain = allowDockingInMain;
}

bool duin::WindowState::GetAllowDockingInMain()
{
    return allowDockingInMain;
}

void duin::WindowState::SetSDLWindow(SDL_Window *sdlWindow)
{
    this->sdlWindow = sdlWindow;
}

SDL_Window *duin::WindowState::GetSDLWindow()
{
    return sdlWindow;
}

void duin::WindowState::SetSDLSurface(SDL_Surface *sdlSurface)
{
    this->sdlSurface = sdlSurface;
}

SDL_Surface *duin::WindowState::GetSDLSurface()
{
    return sdlSurface;
}

void duin::WindowState::SetSDLWindowFlags(SDL_WindowFlags sdlWindowFlags)
{
    if (!IsWindowValid())
    {
        DN_CORE_WARN("WindowState::SetSDLWindowFlags called on invalid window.");
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

SDL_WindowFlags duin::WindowState::GetSDLWindowFlags()
{
    if (!IsWindowValid())
    {
        return 0;
    }
    return ::SDL_GetWindowFlags(sdlWindow);
}

void duin::WindowState::SetTargetRenderFramerate(int targetRenderFramerate)
{
    this->targetRenderFramerate = targetRenderFramerate;
}

int duin::WindowState::GetTargetRenderFramerate()
{
    return targetRenderFramerate;
}

void duin::WindowState::ProcessWindowEvent(WindowEvent event)
{
}
