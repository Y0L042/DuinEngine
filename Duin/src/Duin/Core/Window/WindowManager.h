#pragma once

#include "WindowCtx.h"
#include <SDL3/SDL_video.h>

namespace duin
{
class WindowManager
{
  public:
    static WindowManager &Get();

    static std::shared_ptr<WindowCtx> CreateNewWindow(const std::string &title, int width, int height, uint64_t flags = 0);

    void SubscribeToWindowEvents();

    SDL_Window *GetPrimarySDLWindow()
    {
        if (primaryWindow != nullptr)
        {
            return primaryWindow->GetSDLWindow();
        }
        return nullptr;
    }

    void SetPrimaryWindow(std::shared_ptr<WindowCtx> windowCtx);

    SDL_Surface *GetPrimarySDLSurface()
    {
        if (primaryWindow != nullptr)
        {
            return primaryWindow->GetSDLSurface();
        }
        return nullptr;
    }

  private:
    std::shared_ptr<WindowCtx> primaryWindow;
    std::unordered_map<SDL_Window *, std::shared_ptr<WindowCtx>> sdlWindows;

    void OnWindowEvent(WindowEvent event);
};

} // namespace duin