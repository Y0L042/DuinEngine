#include "dnpch.h"
#include "WindowState.h"

void duin::WindowState::SetSize(int width, int height)
{
    this->width = width;
    this->height = height;
}

void duin::WindowState::GetSize(int &width, int &height)
{
    width = this->width;
    height = this->height;
}

int duin::WindowState::GetWidth()
{
    return width;
}

int duin::WindowState::GetHeight()
{
    return height;
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
    this->sdlWindowFlags = sdlWindowFlags;
}

SDL_WindowFlags duin::WindowState::GetSDLWindowFlags()
{
    return sdlWindowFlags;
}

void duin::WindowState::SetTargetRenderFramerate(int targetRenderFramerate)
{
    this->targetRenderFramerate = targetRenderFramerate;
}

int duin::WindowState::GetTargetRenderFramerate()
{
    return targetRenderFramerate;
}
