#include <doctest.h>
#include <Duin/Core/Events/EventsModule.h>
#include <Duin/Core/Events/EngineInput.h>

#include <SDL3/SDL.h>

namespace TestInputDevice_Mouse
{

static void PushMouseButton(Uint8 button, bool down)
{
    SDL_Event e = {};
    e.type = down ? SDL_EVENT_MOUSE_BUTTON_DOWN : SDL_EVENT_MOUSE_BUTTON_UP;
    e.button.button = button;
    duin::Input::ProcessSDLMouseEvent(e);
}

// ============================================================================
// InputDevice_Mouse::GetEvent routing
// ============================================================================

TEST_SUITE("InputDevice_Mouse - GetEvent")
{
    TEST_CASE("GetEvent PRESSED - routes to IsMouseButtonPressed")
    {
        duin::InputDevice_Mouse mouse;

        duin::Input::CacheCurrentMouseKeyState();
        duin::Input::ClearCurrentMouseKeyState();
        PushMouseButton(SDL_BUTTON_LEFT, true);

        CHECK(mouse.GetEvent(DN_MOUSE_BUTTON_LEFT, duin::Input::KeyEvent::PRESSED, DN_KEY_MOD_NONE));

        // cleanup
        duin::Input::CacheCurrentMouseKeyState();
        duin::Input::ClearCurrentMouseKeyState();
    }

    TEST_CASE("GetEvent PRESSED - false when button not pressed")
    {
        duin::InputDevice_Mouse mouse;

        duin::Input::CacheCurrentMouseKeyState();
        duin::Input::ClearCurrentMouseKeyState();

        CHECK_FALSE(mouse.GetEvent(DN_MOUSE_BUTTON_LEFT, duin::Input::KeyEvent::PRESSED, DN_KEY_MOD_NONE));
    }

    TEST_CASE("GetEvent HELD - routes to IsMouseButtonDown")
    {
        duin::InputDevice_Mouse mouse;

        duin::Input::CacheCurrentMouseKeyState();
        duin::Input::ClearCurrentMouseKeyState();
        PushMouseButton(SDL_BUTTON_LEFT, true);

        CHECK(mouse.GetEvent(DN_MOUSE_BUTTON_LEFT, duin::Input::KeyEvent::HELD, DN_KEY_MOD_NONE));

        // cleanup
        duin::Input::CacheCurrentMouseKeyState();
        duin::Input::ClearCurrentMouseKeyState();
    }

    TEST_CASE("GetEvent HELD - false when button not held")
    {
        duin::InputDevice_Mouse mouse;

        duin::Input::CacheCurrentMouseKeyState();
        duin::Input::ClearCurrentMouseKeyState();

        CHECK_FALSE(mouse.GetEvent(DN_MOUSE_BUTTON_LEFT, duin::Input::KeyEvent::HELD, DN_KEY_MOD_NONE));
    }

    TEST_CASE("GetEvent RELEASED - routes to IsMouseButtonReleased")
    {
        duin::InputDevice_Mouse mouse;

        PushMouseButton(SDL_BUTTON_RIGHT, true);
        duin::Input::CacheCurrentMouseKeyState(); // prev=DOWN
        duin::Input::ClearCurrentMouseKeyState(); // cur=UP

        CHECK(mouse.GetEvent(DN_MOUSE_BUTTON_RIGHT, duin::Input::KeyEvent::RELEASED, DN_KEY_MOD_NONE));

        // cleanup
        duin::Input::CacheCurrentMouseKeyState();
        duin::Input::ClearCurrentMouseKeyState();
    }

    TEST_CASE("GetEvent RELEASED - false when button was not previously down")
    {
        duin::InputDevice_Mouse mouse;

        duin::Input::CacheCurrentMouseKeyState();
        duin::Input::ClearCurrentMouseKeyState();

        CHECK_FALSE(
            mouse.GetEvent(DN_MOUSE_BUTTON_RIGHT, duin::Input::KeyEvent::RELEASED, DN_KEY_MOD_NONE));
    }

    TEST_CASE("GetEvent IDLE - routes to IsMouseButtonUp")
    {
        duin::InputDevice_Mouse mouse;

        duin::Input::CacheCurrentMouseKeyState();
        duin::Input::ClearCurrentMouseKeyState();

        CHECK(mouse.GetEvent(DN_MOUSE_BUTTON_LEFT, duin::Input::KeyEvent::IDLE, DN_KEY_MOD_NONE));
    }

    TEST_CASE("GetEvent IDLE - false when button is down")
    {
        duin::InputDevice_Mouse mouse;

        duin::Input::CacheCurrentMouseKeyState();
        duin::Input::ClearCurrentMouseKeyState();
        PushMouseButton(SDL_BUTTON_LEFT, true);

        CHECK_FALSE(mouse.GetEvent(DN_MOUSE_BUTTON_LEFT, duin::Input::KeyEvent::IDLE, DN_KEY_MOD_NONE));

        // cleanup
        duin::Input::CacheCurrentMouseKeyState();
        duin::Input::ClearCurrentMouseKeyState();
    }

    TEST_CASE("GetEvent PRESSED_REPEATED - falls back to PRESSED behavior")
    {
        duin::InputDevice_Mouse mouse;

        duin::Input::CacheCurrentMouseKeyState();
        duin::Input::ClearCurrentMouseKeyState();
        PushMouseButton(SDL_BUTTON_MIDDLE, true);

        CHECK(mouse.GetEvent(DN_MOUSE_BUTTON_MIDDLE, duin::Input::KeyEvent::PRESSED_REPEATED,
                             DN_KEY_MOD_NONE));

        // cleanup
        duin::Input::CacheCurrentMouseKeyState();
        duin::Input::ClearCurrentMouseKeyState();
    }
}

} // namespace TestInputDevice_Mouse
