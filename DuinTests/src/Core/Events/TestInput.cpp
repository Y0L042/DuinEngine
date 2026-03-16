#include <doctest.h>
#include <Duin/Core/Events/EventsModule.h>
#include <Duin/Core/Events/EngineInput.h>
#include <Duin/Core/Maths/MathsModule.h>

#include <SDL3/SDL.h>

namespace TestInput
{

// Helper: build and process a keyboard SDL event
static void PushKey(SDL_Scancode sc, bool down)
{
    SDL_Event e = {};
    e.type = down ? SDL_EVENT_KEY_DOWN : SDL_EVENT_KEY_UP;
    e.key.scancode = sc;
    e.key.down = down;
    duin::Input::ProcessSDLKeyboardEvent(e);
}

static void PushMouseButton(Uint8 button, bool down)
{
    SDL_Event e = {};
    e.type = down ? SDL_EVENT_MOUSE_BUTTON_DOWN : SDL_EVENT_MOUSE_BUTTON_UP;
    e.button.button = button;
    duin::Input::ProcessSDLMouseEvent(e);
}

// ============================================================================
// Keyboard state
// ============================================================================

TEST_SUITE("Input - Keyboard")
{
    TEST_CASE("ClearCurrentKeyState - IsKeyDown returns false after clear")
    {
        PushKey(SDL_SCANCODE_A, true);
        duin::Input::CacheCurrentKeyState();
        duin::Input::ClearCurrentKeyState();

        CHECK_FALSE(duin::Input::IsKeyDown(DN_SCANCODE_A));

        // cleanup
        duin::Input::ClearCurrentKeyState();
        duin::Input::CacheCurrentKeyState();
        duin::Input::ClearCurrentKeyState();
    }

    TEST_CASE("CacheCurrentKeyState - previous state preserved as current after cache")
    {
        // Set current key DOWN, cache it (prev = DOWN), then clear current (cur = UP)
        PushKey(SDL_SCANCODE_B, true);
        duin::Input::CacheCurrentKeyState(); // prev = DOWN
        duin::Input::ClearCurrentKeyState(); // cur = UP

        // IsKeyReleased: prev=DOWN cur=UP -> true
        CHECK(duin::Input::IsKeyReleased(DN_SCANCODE_B));

        // cleanup
        duin::Input::CacheCurrentKeyState();
        duin::Input::ClearCurrentKeyState();
    }

    TEST_CASE("IsKeyPressed - true only on frame transition UP->DOWN")
    {
        // Ensure clean state: prev=UP, cur=UP
        duin::Input::CacheCurrentKeyState();
        duin::Input::ClearCurrentKeyState();

        // Set current DOWN (prev=UP, cur=DOWN -> pressed)
        PushKey(SDL_SCANCODE_W, true);
        CHECK(duin::Input::IsKeyPressed(DN_SCANCODE_W));

        // cleanup
        duin::Input::CacheCurrentKeyState();
        duin::Input::ClearCurrentKeyState();
    }

    TEST_CASE("IsKeyPressed - false when key held (prev=DOWN, cur=DOWN)")
    {
        // Frame 1: set DOWN
        PushKey(SDL_SCANCODE_W, true);
        duin::Input::CacheCurrentKeyState(); // prev = DOWN

        // Frame 2: still DOWN
        PushKey(SDL_SCANCODE_W, true);
        CHECK_FALSE(duin::Input::IsKeyPressed(DN_SCANCODE_W));

        // cleanup
        duin::Input::CacheCurrentKeyState();
        duin::Input::ClearCurrentKeyState();
    }

    TEST_CASE("IsKeyDown - true when current state is DOWN")
    {
        duin::Input::CacheCurrentKeyState();
        duin::Input::ClearCurrentKeyState();

        PushKey(SDL_SCANCODE_S, true);
        CHECK(duin::Input::IsKeyDown(DN_SCANCODE_S));

        // cleanup
        duin::Input::CacheCurrentKeyState();
        duin::Input::ClearCurrentKeyState();
    }

    TEST_CASE("IsKeyDown - false when current state is UP")
    {
        duin::Input::CacheCurrentKeyState();
        duin::Input::ClearCurrentKeyState();

        CHECK_FALSE(duin::Input::IsKeyDown(DN_SCANCODE_S));
    }

    TEST_CASE("IsKeyReleased - true on transition DOWN->UP")
    {
        // prev = DOWN, cur = UP
        PushKey(SDL_SCANCODE_D, true);
        duin::Input::CacheCurrentKeyState(); // prev = DOWN
        duin::Input::ClearCurrentKeyState(); // cur = UP

        CHECK(duin::Input::IsKeyReleased(DN_SCANCODE_D));

        // cleanup
        duin::Input::CacheCurrentKeyState();
        duin::Input::ClearCurrentKeyState();
    }

    TEST_CASE("IsKeyReleased - false when key was not pressed")
    {
        duin::Input::CacheCurrentKeyState();
        duin::Input::ClearCurrentKeyState();

        CHECK_FALSE(duin::Input::IsKeyReleased(DN_SCANCODE_D));
    }

    TEST_CASE("IsKeyUp - true when current state is UP")
    {
        duin::Input::CacheCurrentKeyState();
        duin::Input::ClearCurrentKeyState();

        CHECK(duin::Input::IsKeyUp(DN_SCANCODE_F));
    }

    TEST_CASE("IsKeyUp - false when current state is DOWN")
    {
        duin::Input::CacheCurrentKeyState();
        duin::Input::ClearCurrentKeyState();

        PushKey(SDL_SCANCODE_F, true);
        CHECK_FALSE(duin::Input::IsKeyUp(DN_SCANCODE_F));

        // cleanup
        duin::Input::CacheCurrentKeyState();
        duin::Input::ClearCurrentKeyState();
    }

    TEST_CASE("IsInputVectorPressed - true when any directional key is down")
    {
        duin::Input::CacheCurrentKeyState();
        duin::Input::ClearCurrentKeyState();

        PushKey(SDL_SCANCODE_W, true);
        CHECK(duin::Input::IsInputVectorPressed(
            DN_SCANCODE_W, DN_SCANCODE_S, DN_SCANCODE_A, DN_SCANCODE_D));

        // cleanup
        duin::Input::CacheCurrentKeyState();
        duin::Input::ClearCurrentKeyState();
    }

    TEST_CASE("IsInputVectorPressed - false when no directional key is down")
    {
        duin::Input::CacheCurrentKeyState();
        duin::Input::ClearCurrentKeyState();

        CHECK_FALSE(duin::Input::IsInputVectorPressed(
            DN_SCANCODE_W, DN_SCANCODE_S, DN_SCANCODE_A, DN_SCANCODE_D));
    }

    TEST_CASE("GetInputVector - zero vector when no keys pressed")
    {
        duin::Input::CacheCurrentKeyState();
        duin::Input::ClearCurrentKeyState();

        duin::Vector2 v = duin::Input::GetInputVector(
            DN_SCANCODE_W, DN_SCANCODE_S, DN_SCANCODE_A, DN_SCANCODE_D);
        CHECK(v.x == 0.0f);
        CHECK(v.y == 0.0f);
    }

    TEST_CASE("GetInputVector - non-zero vector when directional key pressed")
    {
        duin::Input::CacheCurrentKeyState();
        duin::Input::ClearCurrentKeyState();

        PushKey(SDL_SCANCODE_W, true);
        duin::Vector2 v = duin::Input::GetInputVector(
            DN_SCANCODE_W, DN_SCANCODE_S, DN_SCANCODE_A, DN_SCANCODE_D);
        // W = up. GetInputVector maps: down-up = Y, left-right = X (negated right)
        // W down means IsKeyDown(up)=1, Y = down-up = 0-1 = -1 -> normalized = (0,-1)
        CHECK(v.y != 0.0f);

        // cleanup
        duin::Input::CacheCurrentKeyState();
        duin::Input::ClearCurrentKeyState();
    }
}

// ============================================================================
// Mouse state
// ============================================================================

TEST_SUITE("Input - Mouse")
{
    TEST_CASE("IsMouseButtonPressed - true on DOWN when previous was UP")
    {
        duin::Input::CacheCurrentMouseKeyState();
        duin::Input::ClearCurrentMouseKeyState();

        PushMouseButton(SDL_BUTTON_LEFT, true);
        CHECK(duin::Input::IsMouseButtonPressed(DN_MOUSE_BUTTON_LEFT));

        // cleanup
        duin::Input::CacheCurrentMouseKeyState();
        duin::Input::ClearCurrentMouseKeyState();
    }

    TEST_CASE("IsMouseButtonPressed - false when button held (prev=DOWN, cur=DOWN)")
    {
        PushMouseButton(SDL_BUTTON_LEFT, true);
        duin::Input::CacheCurrentMouseKeyState(); // prev = DOWN
        PushMouseButton(SDL_BUTTON_LEFT, true);   // cur = DOWN

        CHECK_FALSE(duin::Input::IsMouseButtonPressed(DN_MOUSE_BUTTON_LEFT));

        // cleanup
        duin::Input::CacheCurrentMouseKeyState();
        duin::Input::ClearCurrentMouseKeyState();
    }

    TEST_CASE("IsMouseButtonDown - true when button held")
    {
        duin::Input::CacheCurrentMouseKeyState();
        duin::Input::ClearCurrentMouseKeyState();

        PushMouseButton(SDL_BUTTON_LEFT, true);
        CHECK(duin::Input::IsMouseButtonDown(DN_MOUSE_BUTTON_LEFT));

        // cleanup
        duin::Input::CacheCurrentMouseKeyState();
        duin::Input::ClearCurrentMouseKeyState();
    }

    TEST_CASE("IsMouseButtonDown - false when button not pressed")
    {
        duin::Input::CacheCurrentMouseKeyState();
        duin::Input::ClearCurrentMouseKeyState();

        CHECK_FALSE(duin::Input::IsMouseButtonDown(DN_MOUSE_BUTTON_LEFT));
    }

    TEST_CASE("IsMouseButtonReleased - true on UP when previous was DOWN")
    {
        PushMouseButton(SDL_BUTTON_RIGHT, true);
        duin::Input::CacheCurrentMouseKeyState(); // prev = DOWN
        duin::Input::ClearCurrentMouseKeyState(); // cur = UP (no event)

        CHECK(duin::Input::IsMouseButtonReleased(DN_MOUSE_BUTTON_RIGHT));

        // cleanup
        duin::Input::CacheCurrentMouseKeyState();
        duin::Input::ClearCurrentMouseKeyState();
    }

    TEST_CASE("IsMouseButtonReleased - false when button was not pressed")
    {
        duin::Input::CacheCurrentMouseKeyState();
        duin::Input::ClearCurrentMouseKeyState();

        CHECK_FALSE(duin::Input::IsMouseButtonReleased(DN_MOUSE_BUTTON_RIGHT));
    }

    TEST_CASE("IsMouseButtonUp - true when button not pressed")
    {
        duin::Input::CacheCurrentMouseKeyState();
        duin::Input::ClearCurrentMouseKeyState();

        CHECK(duin::Input::IsMouseButtonUp(DN_MOUSE_BUTTON_LEFT));
    }

    TEST_CASE("IsMouseButtonUp - false when button is down")
    {
        duin::Input::CacheCurrentMouseKeyState();
        duin::Input::ClearCurrentMouseKeyState();

        PushMouseButton(SDL_BUTTON_LEFT, true);
        CHECK_FALSE(duin::Input::IsMouseButtonUp(DN_MOUSE_BUTTON_LEFT));

        // cleanup
        duin::Input::CacheCurrentMouseKeyState();
        duin::Input::ClearCurrentMouseKeyState();
    }
}

} // namespace TestInput
