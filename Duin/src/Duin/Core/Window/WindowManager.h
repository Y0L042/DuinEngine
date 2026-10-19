#pragma once

#include "WindowCtx.h"
#include "Duin/Core/Signals/Signal.h"
#include <SDL3/SDL_video.h>
#include <memory>
#include <vector>

namespace duin
{
class EventHandler;
class WindowManager
{
  public:
    Signal<> OnPrimaryWindowClosing;
    Signal<std::shared_ptr<WindowCtx>> OnWindowCreated;
    Signal<std::shared_ptr<WindowCtx>> OnWindowDestroying;

    void SetEventHandler(EventHandler *eventHandler);

    std::shared_ptr<WindowCtx> CreateNewWindow(const std::string &title, int width, int height, uint64_t flags = 0);
    bool DestroyWindow(SDL_Window *window);

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

    std::shared_ptr<WindowCtx> GetPrimaryWindow()
    {
        return primaryWindow;
    }

    std::shared_ptr<WindowCtx> GetWindowCtx(SDL_Window *window);
    std::vector<std::shared_ptr<WindowCtx>> GetSecondaryWindows();

    void ReleasePendingWindows();
    void DestroyPendingWindows();
    void DestroyAllWindows();

    void ExecuteWindowDraw();
    void ExecuteWindowDrawUI();

    SDL_Surface *GetPrimarySDLSurface()
    {
        if (primaryWindow != nullptr)
        {
            return primaryWindow->GetSDLSurface();
        }
        return nullptr;
    }

  private:
    EventHandler *eventHandler = nullptr;
    std::shared_ptr<WindowCtx> primaryWindow;
    std::unordered_map<SDL_Window *, std::shared_ptr<WindowCtx>> sdlWindows;
    std::vector<std::shared_ptr<WindowCtx>> pendingRelease;
    std::vector<std::shared_ptr<WindowCtx>> pendingDestroy;

    void OnWindowEvent(WindowEvent event);
};

} // namespace duin