#include <doctest.h>
#include <Duin/Core/Events/EventsModule.h>
#include <Duin/Core/Events/EngineInput.h>

#include <SDL3/SDL.h>

namespace TestInputDevice_Keyboard
{

static void PushKey(SDL_Scancode sc, bool down)
{
    SDL_Event e = {};
    e.type = down ? SDL_EVENT_KEY_DOWN : SDL_EVENT_KEY_UP;
    e.key.scancode = sc;
    e.key.down = down;
    duin::Input::ProcessSDLKeyboardEvent(e);
}

// ============================================================================
// InputDevice_Keyboard::GetEvent routing
// ============================================================================

TEST_SUITE("InputDevice_Keyboard - GetEvent")
{
    TEST_CASE("GetEvent PRESSED - returns true on key press transition")
    {
        duin::InputDevice_Keyboard kb;

        duin::Input::CacheCurrentKeyState();
        duin::Input::ClearCurrentKeyState();
        PushKey(SDL_SCANCODE_W, true); // prev=UP, cur=DOWN

        CHECK(kb.GetEvent(DN_SCANCODE_W, duin::Input::KeyEvent::PRESSED, DN_KEY_MOD_NONE));

        // cleanup
        duin::Input::CacheCurrentKeyState();
        duin::Input::ClearCurrentKeyState();
    }

    TEST_CASE("GetEvent PRESSED - returns false when key not pressed")
    {
        duin::InputDevice_Keyboard kb;

        duin::Input::CacheCurrentKeyState();
        duin::Input::ClearCurrentKeyState();

        CHECK_FALSE(kb.GetEvent(DN_SCANCODE_W, duin::Input::KeyEvent::PRESSED, DN_KEY_MOD_NONE));
    }

    TEST_CASE("GetEvent HELD - returns true when key is down")
    {
        duin::InputDevice_Keyboard kb;

        duin::Input::CacheCurrentKeyState();
        duin::Input::ClearCurrentKeyState();
        PushKey(SDL_SCANCODE_S, true);

        CHECK(kb.GetEvent(DN_SCANCODE_S, duin::Input::KeyEvent::HELD, DN_KEY_MOD_NONE));

        // cleanup
        duin::Input::CacheCurrentKeyState();
        duin::Input::ClearCurrentKeyState();
    }

    TEST_CASE("GetEvent HELD - returns false when key is up")
    {
        duin::InputDevice_Keyboard kb;

        duin::Input::CacheCurrentKeyState();
        duin::Input::ClearCurrentKeyState();

        CHECK_FALSE(kb.GetEvent(DN_SCANCODE_S, duin::Input::KeyEvent::HELD, DN_KEY_MOD_NONE));
    }

    TEST_CASE("GetEvent RELEASED - returns true on key release transition")
    {
        duin::InputDevice_Keyboard kb;

        PushKey(SDL_SCANCODE_D, true);
        duin::Input::CacheCurrentKeyState(); // prev=DOWN
        duin::Input::ClearCurrentKeyState(); // cur=UP

        CHECK(kb.GetEvent(DN_SCANCODE_D, duin::Input::KeyEvent::RELEASED, DN_KEY_MOD_NONE));

        // cleanup
        duin::Input::CacheCurrentKeyState();
        duin::Input::ClearCurrentKeyState();
    }

    TEST_CASE("GetEvent RELEASED - returns false when key was not previously down")
    {
        duin::InputDevice_Keyboard kb;

        duin::Input::CacheCurrentKeyState();
        duin::Input::ClearCurrentKeyState();

        CHECK_FALSE(kb.GetEvent(DN_SCANCODE_D, duin::Input::KeyEvent::RELEASED, DN_KEY_MOD_NONE));
    }

    TEST_CASE("GetEvent IDLE - returns true when key is up")
    {
        duin::InputDevice_Keyboard kb;

        duin::Input::CacheCurrentKeyState();
        duin::Input::ClearCurrentKeyState();

        CHECK(kb.GetEvent(DN_SCANCODE_F, duin::Input::KeyEvent::IDLE, DN_KEY_MOD_NONE));
    }

    TEST_CASE("GetEvent IDLE - returns false when key is down")
    {
        duin::InputDevice_Keyboard kb;

        duin::Input::CacheCurrentKeyState();
        duin::Input::ClearCurrentKeyState();
        PushKey(SDL_SCANCODE_F, true);

        CHECK_FALSE(kb.GetEvent(DN_SCANCODE_F, duin::Input::KeyEvent::IDLE, DN_KEY_MOD_NONE));

        // cleanup
        duin::Input::CacheCurrentKeyState();
        duin::Input::ClearCurrentKeyState();
    }

    TEST_CASE("Null device InputBinding - Triggered returns false")
    {
        // InputBinding's Triggered() guards against null device - construct with valid device,
        // then null it out to simulate a detached binding.
        auto kb = std::make_shared<duin::InputDevice_Keyboard>();
        duin::InputBinding binding(kb, DN_SCANCODE_A, duin::Input::KeyEvent::PRESSED);
        binding.device = nullptr; // simulate detached device
        CHECK_FALSE(binding.Triggered());
    }
}

} // namespace TestInputDevice_Keyboard
