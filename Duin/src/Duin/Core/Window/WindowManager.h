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

  private:
    std::unordered_map<SDL_Window *, std::shared_ptr<WindowState>> sdlWindows;
};

} // namespace duin