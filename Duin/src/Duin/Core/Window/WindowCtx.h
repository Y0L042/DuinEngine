#pragma once
#include <Duin/Core/Utils/UUID.h>
#include <Duin/Core/Events/Event.h>
#include <Duin/Core/Signals/Signal.h>
#include <Duin/Render/RHI.h>
#include <SDL3/SDL_init.h>
#include <SDL3/SDL_video.h>
#include <string>

struct ImGuiContext;

namespace duin
{
class Camera;

class WindowCtx
{
  public:
    Signal<> OnDraw;
    Signal<> OnDrawUI;

    WindowCtx();
    ~WindowCtx();

    bool IsWindowValid();

    void SetSize(int width, int height);
    void GetSize(int &width, int &height);
    void GetSizeInPixels(int &width, int &height);
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

    void *GetNativeHandle();

    void SetCamera(Camera *camera);
    Camera *GetCamera();

    void SetClearColor(uint32_t rgba);
    uint32_t GetClearColor();

    void SetImGuiContext(::ImGuiContext *imguiContext);
    ::ImGuiContext *GetImGuiContext();

    void SetFrameBuffer(RHIFrameBufferHandle frameBuffer, int width, int height);
    RHIFrameBufferHandle GetFrameBuffer();
    void GetFrameBufferSize(int &width, int &height);

    void SetViewSlot(int viewSlot);
    int GetViewSlot();
    RHIViewId GetSceneView();
    RHIViewId GetUIView();

    void SetPendingDestroy(bool pendingDestroy);
    bool IsPendingDestroy();

    void ProcessWindowEvent(WindowEvent event);

    void Destroy();

  private:
    bool customImguiPath = false;
    std::string imguiPath = "./";
    bool pauseOnMinimize = false;
    bool allowDockingInMain = false;
    SDL_Window *sdlWindow = nullptr;
    SDL_Surface *sdlSurface = nullptr;
    int targetRenderFramerate = 60;
    Camera *camera = nullptr;
    uint32_t clearColor = 0x303030ff;
    ::ImGuiContext *imguiContext = nullptr;
    RHIFrameBufferHandle frameBuffer;
    int frameBufferWidth = 0;
    int frameBufferHeight = 0;
    int viewSlot = -1;
    bool pendingDestroy = false;
};

} // namespace duin
