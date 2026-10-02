#pragma once
#include <SDL3/SDL_init.h>
#include <string>

namespace duin
{
class WindowState
{
  public:
    void SetSize(int width, int height);
    void GetSize(int &width, int &height);
    int GetWidth();
    int GetHeight();

    void UseCustomImguiPath(bool customImguiPath);
    bool UsingCustomImguiPath();

    void SetImguiPath(const std::string &imguiPath);
    const std::string &GetImguiPath();

    void SetPauseOnMinimize(bool pauseOnMinimize);
    bool GetPauseOnMinimize();

    void SetAllowDockingInMain(bool allowDockingInMain);
    bool GetAllowDockingInMain();

    void SetSDLWindow(SDL_Window *sdlWindow);
    SDL_Window *GetSDLWindow();

    void SetSDLSurface(SDL_Surface *sdlSurface);
    SDL_Surface *GetSDLSurface();

    void SetSDLWindowFlags(SDL_WindowFlags sdlWindowFlags);
    SDL_WindowFlags GetSDLWindowFlags();

    void SetTargetRenderFramerate(int targetRenderFramerate);
    int GetTargetRenderFramerate();

  private:
    int width = 1280;
    int height = 720;
    bool customImguiPath = false;
    std::string imguiPath = "./";
    bool pauseOnMinimize = false;
    bool allowDockingInMain = false;
    SDL_Window *sdlWindow = NULL;
    SDL_Surface *sdlSurface = NULL;
    SDL_WindowFlags sdlWindowFlags = 0;
    int targetRenderFramerate = 60;
};

} // namespace duin
