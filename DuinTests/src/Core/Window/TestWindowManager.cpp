#include <doctest.h>
#include <Duin/Core/Window/WindowManager.h>
#include <Duin/Core/Window/WindowCtx.h>
#include <Duin/Core/Events/EventHandler.h>
#include <SDL3/SDL.h>
#include <memory>
#include <unordered_map>

namespace TestWindowManager
{
TEST_SUITE("WindowManager - Without SDL video")
{
    TEST_CASE("Fresh manager has no windows")
    {
        duin::WindowManager wm;
        CHECK(wm.GetPrimaryWindow() == nullptr);
        CHECK(wm.GetPrimarySDLWindow() == nullptr);
        CHECK(wm.GetPrimarySDLSurface() == nullptr);
        CHECK(wm.GetSecondaryWindows().empty());
        CHECK(wm.GetWindowCtx(nullptr) == nullptr);
    }

    TEST_CASE("DestroyWindow of an unknown window returns false")
    {
        duin::WindowManager wm;
        int closing = 0;
        auto conn = wm.OnPrimaryWindowClosing.Connect([&]() { closing++; });

        CHECK_FALSE(wm.DestroyWindow(nullptr));
        CHECK(closing == 0);
    }

    TEST_CASE("Destroying the primary window emits OnPrimaryWindowClosing once")
    {
        duin::WindowManager wm;
        auto primary = std::make_shared<duin::WindowCtx>();
        wm.SetPrimaryWindow(primary);
        REQUIRE(wm.GetPrimaryWindow() == primary);

        int closing = 0;
        auto conn = wm.OnPrimaryWindowClosing.Connect([&]() { closing++; });

        CHECK(wm.DestroyWindow(primary->GetSDLWindow()));
        CHECK(closing == 1);
        CHECK(wm.GetPrimaryWindow() == nullptr);

        CHECK_FALSE(wm.DestroyWindow(primary->GetSDLWindow()));
        CHECK(closing == 1);
    }

    TEST_CASE("Primary window is not reported as secondary")
    {
        duin::WindowManager wm;
        wm.SetPrimaryWindow(std::make_shared<duin::WindowCtx>());
        CHECK(wm.GetSecondaryWindows().empty());
    }

    TEST_CASE("Release, destroy-pending and destroy-all are safe on empty state")
    {
        duin::WindowManager wm;
        int destroying = 0;
        auto conn = wm.OnWindowDestroying.Connect([&](std::shared_ptr<duin::WindowCtx>) { destroying++; });

        wm.ReleasePendingWindows();
        wm.DestroyPendingWindows();
        wm.DestroyAllWindows();

        CHECK(destroying == 0);
        CHECK(wm.GetPrimaryWindow() == nullptr);
    }

    TEST_CASE("DestroyAllWindows clears the primary window")
    {
        duin::WindowManager wm;
        wm.SetPrimaryWindow(std::make_shared<duin::WindowCtx>());
        wm.DestroyAllWindows();
        CHECK(wm.GetPrimaryWindow() == nullptr);
    }

    TEST_CASE("CreateNewWindow without SDL video returns nullptr")
    {
        if (::SDL_WasInit(SDL_INIT_VIDEO) != 0)
        {
            MESSAGE("SDL video is initialised by an earlier test; skipping");
            return;
        }

        ::SDL_SetHint(SDL_HINT_VIDEO_DRIVER, "duin_no_such_driver");

        duin::WindowManager wm;
        int created = 0;
        auto conn = wm.OnWindowCreated.Connect([&](std::shared_ptr<duin::WindowCtx>) { created++; });

        CHECK(wm.CreateNewWindow("NoVideo", 64, 64) == nullptr);
        CHECK(created == 0);
        CHECK(wm.GetPrimaryWindow() == nullptr);

        ::SDL_ResetHint(SDL_HINT_VIDEO_DRIVER);
        if (::SDL_WasInit(SDL_INIT_VIDEO) != 0)
        {
            ::SDL_QuitSubSystem(SDL_INIT_VIDEO);
        }
    }

    TEST_CASE("SetEventHandler subscribes to close requests")
    {
        duin::EventHandler handler;
        duin::WindowManager wm;
        wm.SetEventHandler(&handler);

        SDL_Event e = {};
        e.type = SDL_EVENT_WINDOW_CLOSE_REQUESTED;
        e.window.windowID = 987654;
        handler.PollEvent(e);

        CHECK(wm.GetPrimaryWindow() == nullptr);
    }

    TEST_CASE("SetEventHandler with nullptr does not subscribe")
    {
        duin::WindowManager wm;
        wm.SetEventHandler(nullptr);
        CHECK(wm.GetPrimaryWindow() == nullptr);
    }
}
} // namespace TestWindowManager
