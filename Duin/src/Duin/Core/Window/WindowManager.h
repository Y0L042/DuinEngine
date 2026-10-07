#pragma once

#include "WindowState.h"
#include <SDL3/SDL_video.h>

namespace duin
{
class WindowManager
{
  public:
    static WindowManager &Get();

    static std::shared_ptr<WindowState> CreateNewWindow(int width, int height, const std::string &title = "Window");

    void SubscribeToWindowEvents(); 
    SDL_Window* GetPrimarySDLWindow()
    {
        if (primaryWindow != nullptr)
        {
            return primaryWindow->GetSDLWindow();
        }
        return nullptr;
    }

  private:
    std::shared_ptr<WindowState> primaryWindow;
    std::unordered_map<SDL_Window *, std::shared_ptr<WindowState>> sdlWindows;

    void OnWindowEvent(WindowEvent event);
};

} // namespace duin