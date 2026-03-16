#include <doctest.h>
#include <Duin/Core/Events/EventsModule.h>
#include <Duin/Core/Events/EngineInput.h>

#include <SDL3/SDL.h>

namespace TestInputActions
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

// ============================================================================
// InputActions API
// ============================================================================

TEST_SUITE("InputActions")
{
    TEST_CASE("CreateInputAction - action can be queried after creation")
    {
        duin::CreateInputAction("test_create");
        // No bindings -> never triggered
        CHECK_FALSE(duin::IsInputActionTriggered("test_create"));

        // cleanup
        duin::RemoveInputAction("test_create");
    }

    TEST_CASE("CreateInputAction - duplicate name is a no-op")
    {
        duin::CreateInputAction("test_dup");
        duin::CreateInputAction("test_dup"); // second call is a no-op
        // Action still exists and is queryable
        CHECK_FALSE(duin::IsInputActionTriggered("test_dup"));

        // cleanup
        duin::RemoveInputAction("test_dup");
    }

    TEST_CASE("AddInputActionBinding - binding is added to action")
    {
        auto kb = duin::GetKeyboard_01();
        duin::CreateInputAction("test_bind");
        duin::AddInputActionBinding("test_bind", kb, DN_SCANCODE_SPACE, duin::Input::KeyEvent::PRESSED);

        // Key not pressed -> not triggered
        duin::Input::CacheCurrentKeyState();
        duin::Input::ClearCurrentKeyState();
        CHECK_FALSE(duin::IsInputActionTriggered("test_bind"));

        // cleanup
        duin::RemoveInputAction("test_bind");
        duin::Input::CacheCurrentKeyState();
        duin::Input::ClearCurrentKeyState();
    }

    TEST_CASE("AddInputActionBinding - duplicate binding is a no-op (same hash not added twice)")
    {
        auto kb = duin::GetKeyboard_01();
        duin::CreateInputAction("test_dup_bind");
        duin::AddInputActionBinding("test_dup_bind", kb, DN_SCANCODE_A, duin::Input::KeyEvent::PRESSED);
        duin::AddInputActionBinding("test_dup_bind", kb, DN_SCANCODE_A, duin::Input::KeyEvent::PRESSED);

        // Still valid, just a duplicate was silently ignored
        duin::Input::CacheCurrentKeyState();
        duin::Input::ClearCurrentKeyState();
        CHECK_FALSE(duin::IsInputActionTriggered("test_dup_bind"));

        // cleanup
        duin::RemoveInputAction("test_dup_bind");
        duin::Input::CacheCurrentKeyState();
        duin::Input::ClearCurrentKeyState();
    }

    TEST_CASE("RemoveInputActionBinding - removes specific binding, action remains")
    {
        auto kb = duin::GetKeyboard_01();
        duin::CreateInputAction("test_rm_bind");
        duin::AddInputActionBinding("test_rm_bind", kb, DN_SCANCODE_A, duin::Input::KeyEvent::PRESSED);
        duin::AddInputActionBinding("test_rm_bind", kb, DN_SCANCODE_B, duin::Input::KeyEvent::PRESSED);

        duin::RemoveInputActionBinding("test_rm_bind", kb, DN_SCANCODE_A, duin::Input::KeyEvent::PRESSED);

        // Action still exists (B binding remains), still queryable
        duin::Input::CacheCurrentKeyState();
        duin::Input::ClearCurrentKeyState();
        CHECK_FALSE(duin::IsInputActionTriggered("test_rm_bind"));

        // cleanup
        duin::RemoveInputAction("test_rm_bind");
        duin::Input::CacheCurrentKeyState();
        duin::Input::ClearCurrentKeyState();
    }

    TEST_CASE("RemoveInputAction - action no longer triggered after removal")
    {
        duin::CreateInputAction("test_rm");
        duin::RemoveInputAction("test_rm");

        // After removal, IsInputActionTriggered should return false gracefully
        CHECK_FALSE(duin::IsInputActionTriggered("test_rm"));
    }

    TEST_CASE("IsInputActionTriggered - false when device state not active")
    {
        auto kb = duin::GetKeyboard_01();
        duin::CreateInputAction("test_not_triggered");
        duin::AddInputActionBinding("test_not_triggered", kb, DN_SCANCODE_Z, duin::Input::KeyEvent::PRESSED);

        duin::Input::CacheCurrentKeyState();
        duin::Input::ClearCurrentKeyState();
        CHECK_FALSE(duin::IsInputActionTriggered("test_not_triggered"));

        // cleanup
        duin::RemoveInputAction("test_not_triggered");
        duin::Input::CacheCurrentKeyState();
        duin::Input::ClearCurrentKeyState();
    }

    TEST_CASE("IsInputActionTriggered - true when bound key is pressed")
    {
        auto kb = duin::GetKeyboard_01();
        duin::CreateInputAction("test_triggered");
        duin::AddInputActionBinding("test_triggered", kb, DN_SCANCODE_G, duin::Input::KeyEvent::PRESSED);

        // Frame: prev=UP, cur=DOWN -> pressed
        duin::Input::CacheCurrentKeyState();
        duin::Input::ClearCurrentKeyState();
        PushKey(SDL_SCANCODE_G, true);

        CHECK(duin::IsInputActionTriggered("test_triggered"));

        // cleanup
        duin::RemoveInputAction("test_triggered");
        duin::Input::CacheCurrentKeyState();
        duin::Input::ClearCurrentKeyState();
    }

    TEST_CASE("IsInputActionTriggered - false when different key pressed")
    {
        auto kb = duin::GetKeyboard_01();
        duin::CreateInputAction("test_wrong_key");
        duin::AddInputActionBinding("test_wrong_key", kb, DN_SCANCODE_H, duin::Input::KeyEvent::PRESSED);

        duin::Input::CacheCurrentKeyState();
        duin::Input::ClearCurrentKeyState();
        PushKey(SDL_SCANCODE_I, true); // different key

        CHECK_FALSE(duin::IsInputActionTriggered("test_wrong_key"));

        // cleanup
        duin::RemoveInputAction("test_wrong_key");
        duin::Input::CacheCurrentKeyState();
        duin::Input::ClearCurrentKeyState();
    }
}

} // namespace TestInputActions
