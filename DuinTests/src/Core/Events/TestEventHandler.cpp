#include <doctest.h>
#include <Duin/Core/Events/EventHandler.h>
#include <Duin/Core/Events/Event.h>
#include <SDL3/SDL.h>
#include <vector>

namespace TestEventHandler
{
static SDL_Event MakeEvent(Uint32 type)
{
    SDL_Event e = {};
    e.type = type;
    return e;
}

TEST_SUITE("EventHandler - Instance dispatch")
{
    TEST_CASE("Keyboard and mouse events reach input listeners")
    {
        duin::EventHandler handler;
        std::vector<Uint32> received;
        handler.RegisterInputEventListener([&](duin::Event event) { received.push_back(event.sdlEvent.type); });

        handler.PollEvent(MakeEvent(SDL_EVENT_KEY_DOWN));
        handler.PollEvent(MakeEvent(SDL_EVENT_KEY_UP));
        handler.PollEvent(MakeEvent(SDL_EVENT_MOUSE_MOTION));
        handler.PollEvent(MakeEvent(SDL_EVENT_MOUSE_BUTTON_DOWN));
        handler.PollEvent(MakeEvent(SDL_EVENT_MOUSE_WHEEL));

        REQUIRE(received.size() == 5);
        CHECK(received[0] == SDL_EVENT_KEY_DOWN);
        CHECK(received[1] == SDL_EVENT_KEY_UP);
        CHECK(received[2] == SDL_EVENT_MOUSE_MOTION);
        CHECK(received[3] == SDL_EVENT_MOUSE_BUTTON_DOWN);
        CHECK(received[4] == SDL_EVENT_MOUSE_WHEEL);
    }

    TEST_CASE("Close requests reach window listeners only")
    {
        duin::EventHandler handler;
        int input = 0;
        std::vector<Uint32> windowEvents;
        handler.RegisterInputEventListener([&](duin::Event) { input++; });
        handler.RegisterWindowEventListener(
            [&](duin::WindowEvent event) { windowEvents.push_back(event.sdlEvent.type); });

        SDL_Event e = MakeEvent(SDL_EVENT_WINDOW_CLOSE_REQUESTED);
        e.window.windowID = 42;
        handler.PollEvent(e);

        CHECK(input == 0);
        REQUIRE(windowEvents.size() == 1);
        CHECK(windowEvents[0] == SDL_EVENT_WINDOW_CLOSE_REQUESTED);
    }

    TEST_CASE("Window listener receives the original window id")
    {
        duin::EventHandler handler;
        SDL_WindowID seen = 0;
        handler.RegisterWindowEventListener([&](duin::WindowEvent event) { seen = event.sdlEvent.window.windowID; });

        SDL_Event e = MakeEvent(SDL_EVENT_WINDOW_CLOSE_REQUESTED);
        e.window.windowID = 1234;
        handler.PollEvent(e);

        CHECK(seen == 1234);
    }

    TEST_CASE("Other events reach no listener")
    {
        duin::EventHandler handler;
        int input = 0, window = 0;
        handler.RegisterInputEventListener([&](duin::Event) { input++; });
        handler.RegisterWindowEventListener([&](duin::WindowEvent) { window++; });

        handler.PollEvent(MakeEvent(SDL_EVENT_QUIT));
        handler.PollEvent(MakeEvent(SDL_EVENT_WINDOW_RESIZED));
        handler.PollEvent(MakeEvent(SDL_EVENT_GAMEPAD_ADDED));

        CHECK(input == 0);
        CHECK(window == 0);
    }

    TEST_CASE("All registered listeners are called in registration order")
    {
        duin::EventHandler handler;
        std::vector<int> order;
        handler.RegisterInputEventListener([&](duin::Event) { order.push_back(1); });
        handler.RegisterInputEventListener([&](duin::Event) { order.push_back(2); });

        handler.PollEvent(MakeEvent(SDL_EVENT_KEY_DOWN));

        REQUIRE(order.size() == 2);
        CHECK(order[0] == 1);
        CHECK(order[1] == 2);
    }

    TEST_CASE("Separate instances keep separate listeners")
    {
        duin::EventHandler a;
        duin::EventHandler b;
        int aCount = 0, bCount = 0;
        a.RegisterInputEventListener([&](duin::Event) { aCount++; });
        b.RegisterInputEventListener([&](duin::Event) { bCount++; });

        a.PollEvent(MakeEvent(SDL_EVENT_KEY_DOWN));
        CHECK(aCount == 1);
        CHECK(bCount == 0);

        b.PollEvent(MakeEvent(SDL_EVENT_KEY_DOWN));
        b.PollEvent(MakeEvent(SDL_EVENT_KEY_DOWN));
        CHECK(aCount == 1);
        CHECK(bCount == 2);
    }

    TEST_CASE("GetPolledEvent forwards to PollEvent")
    {
        duin::EventHandler handler;
        int input = 0, window = 0;
        handler.RegisterInputEventListener([&](duin::Event) { input++; });
        handler.RegisterWindowEventListener([&](duin::WindowEvent) { window++; });

        handler.GetPolledEvent(MakeEvent(SDL_EVENT_KEY_DOWN));
        handler.GetPolledEvent(MakeEvent(SDL_EVENT_WINDOW_CLOSE_REQUESTED));

        CHECK(input == 1);
        CHECK(window == 1);
    }

    TEST_CASE("IsCloseRequested stays false after a close request")
    {
        duin::EventHandler handler;
        CHECK_FALSE(handler.IsCloseRequested());
        handler.PollEvent(MakeEvent(SDL_EVENT_WINDOW_CLOSE_REQUESTED));
        CHECK_FALSE(handler.IsCloseRequested());
    }
}
} // namespace TestEventHandler
