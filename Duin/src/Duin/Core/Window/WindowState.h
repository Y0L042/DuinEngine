#pragma once
#include <Duin/Core/Utils/UUID.h>
#include <SDL3/SDL_init.h>
#include <SDL3/SDL_video.h>
#include <string>

namespace duin
{

class WindowState
{
  public:
    bool IsWindowValid();

    void SetSize(int width, int height);
    void GetSize(int &width, int &height);
    int GetWidth();
    int GetHeight();

    void SetTitle(const std::string &title);
    std::string GetTitle();

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

    void ProcessWindowEvent(WindowEvent event);

  private:
    bool customImguiPath = false;
    std::string imguiPath = "./";
    bool pauseOnMinimize = false;
    bool allowDockingInMain = false;
    SDL_Window *sdlWindow = nullptr;
    SDL_Surface *sdlSurface = nullptr;
    int targetRenderFramerate = 60;
};

} // namespace duin
