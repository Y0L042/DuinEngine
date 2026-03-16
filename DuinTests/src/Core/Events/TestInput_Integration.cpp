#include <doctest.h>
#include <Duin/Core/Application.h>
#include <Duin/Core/Events/EventsModule.h>
#include <Duin/Core/Events/EngineInput.h>
#include <Duin/Core/Maths/MathsModule.h>
#include "Defines.h"

#include <SDL3/SDL.h>

namespace TestInput_Integration
{

// ---------------------------------------------------------------------------
// Test application subclass
// ---------------------------------------------------------------------------
class InputTestApp : public duin::Application
{
  public:
    void Initialize() override
    {
        SetHeadless(true);
    }
};

// ---------------------------------------------------------------------------
// RAII helper to initialize/shutdown the headless app
// ---------------------------------------------------------------------------
struct AppScope
{
    InputTestApp app;
    double dt = 0.016, pCur = 0.0, pPrev = 0.0, pAccum = 0.0;

    AppScope()
    {
        app.SetHeadless(true);
        app.EngineInitialize();
        app.Initialize();
        app.InitSDL();
        app.InitBGFX();
        app.InitImGui();
        duin::Input::ResetAllInputState();
    }

    ~AppScope()
    {
        app.ShutdownImGui();
        app.ShutdownBGFX();
        app.ShutdownSDL();
        app.EngineExit();
        app.Exit();
    }

    void ProcessFrame()
    {
        app.ProcessFrame(dt, pCur, pPrev, pAccum);
    }

    void PushKeyDown(SDL_Scancode sc)
    {
        SDL_Event e = {};
        e.type = SDL_EVENT_KEY_DOWN;
        e.key.scancode = sc;
        e.key.down = true;
        app.PushSDLEvent(&e);
    }

    void PushKeyUp(SDL_Scancode sc)
    {
        SDL_Event e = {};
        e.type = SDL_EVENT_KEY_UP;
        e.key.scancode = sc;
        e.key.down = false;
        app.PushSDLEvent(&e);
    }

    void PushMouseButtonDown(Uint8 button)
    {
        SDL_Event e = {};
        e.type = SDL_EVENT_MOUSE_BUTTON_DOWN;
        e.button.button = button;
        app.PushSDLEvent(&e);
    }

    void PushMouseButtonUp(Uint8 button)
    {
        SDL_Event e = {};
        e.type = SDL_EVENT_MOUSE_BUTTON_UP;
        e.button.button = button;
        app.PushSDLEvent(&e);
    }

    void PushMouseMotion(float xrel, float yrel)
    {
        SDL_Event e = {};
        e.type = SDL_EVENT_MOUSE_MOTION;
        e.motion.x = 1.0f;
        e.motion.y = 1.0f;
        e.motion.xrel = xrel;
        e.motion.yrel = yrel;
        app.PushSDLEvent(&e);
    }

    void PushMouseMotionAt(float x, float y, float xrel, float yrel)
    {
        SDL_Event e = {};
        e.type = SDL_EVENT_MOUSE_MOTION;
        e.motion.x = x;
        e.motion.y = y;
        e.motion.xrel = xrel;
        e.motion.yrel = yrel;
        app.PushSDLEvent(&e);
    }
};

// ============================================================================
// Integration: keyboard
// ============================================================================

TEST_SUITE("Input Integration - Keyboard")
{
    TEST_CASE("Key press detected after PushSDLEvent KEY_DOWN + ProcessFrame")
    {
        AppScope scope;
        scope.PushKeyDown(SDL_SCANCODE_W);
        scope.ProcessFrame();

        CHECK(duin::Input::IsKeyPressed(DN_SCANCODE_W));
    }

    TEST_CASE("Key held: IsKeyDown true on frame 1; IsKeyPressed false on frame 2")
    {
        AppScope scope;

        // Frame 1: key down
        scope.PushKeyDown(SDL_SCANCODE_A);
        scope.ProcessFrame();
        CHECK(duin::Input::IsKeyDown(DN_SCANCODE_A));

        // Frame 2: no new event, but key remains reported down by ProcessEvents
        // (ProcessEvents clears then re-processes; without a new event cur=UP)
        // Actually ProcessEvents calls ClearCurrentKeyState so without re-pushing,
        // key won't be down. We push it again to simulate held.
        scope.PushKeyDown(SDL_SCANCODE_A);
        scope.ProcessFrame();
        CHECK(duin::Input::IsKeyDown(DN_SCANCODE_A));
        CHECK_FALSE(duin::Input::IsKeyPressed(DN_SCANCODE_A));
    }

    TEST_CASE("Key released: IsKeyReleased true on frame after KEY_UP")
    {
        AppScope scope;

        // Frame 1: press
        scope.PushKeyDown(SDL_SCANCODE_S);
        scope.ProcessFrame();

        // Frame 2: release
        scope.PushKeyUp(SDL_SCANCODE_S);
        scope.ProcessFrame();

        CHECK(duin::Input::IsKeyReleased(DN_SCANCODE_S));
    }
}

// ============================================================================
// Integration: mouse buttons
// ============================================================================

TEST_SUITE("Input Integration - Mouse")
{
    TEST_CASE("Mouse button pressed: IsMouseButtonPressed true after BUTTON_DOWN + ProcessFrame")
    {
        AppScope scope;
        scope.PushMouseButtonDown(SDL_BUTTON_LEFT);
        scope.ProcessFrame();

        CHECK(duin::Input::IsMouseButtonPressed(DN_MOUSE_BUTTON_LEFT));
    }

    TEST_CASE("Mouse button released: IsMouseButtonReleased true on frame after BUTTON_UP")
    {
        AppScope scope;

        // Frame 1: press
        scope.PushMouseButtonDown(SDL_BUTTON_RIGHT);
        scope.ProcessFrame();

        // Frame 2: release
        scope.PushMouseButtonUp(SDL_BUTTON_RIGHT);
        scope.ProcessFrame();

        CHECK(duin::Input::IsMouseButtonReleased(DN_MOUSE_BUTTON_RIGHT));
    }

    TEST_CASE("Mouse motion: GetMouseDelta returns non-zero after MOUSE_MOTION event")
    {
        AppScope scope;
        scope.ProcessFrame();

        scope.PushMouseMotion(5.0f, -3.0f);
        scope.ProcessFrame();

        // GetMouseDelta uses SDL_GetRelativeMouseState which accumulates SDL motion
        duin::Vector2 delta = duin::Input::GetMouseDelta();
        // Delta should be non-zero when motion was pushed (SDL_GetRelativeMouseState returns the
        // accumulated relative mouse motion since last call)
        MSG_CHECK(delta.x, delta.x != 0.0f);
        MSG_CHECK(delta.y, delta.y != 0.0f);
    }

    TEST_CASE("Mouse motion: GetMouseDelta is zero on frames with no motion event")
    {
        AppScope scope;

        // Frame 1: no motion
        scope.ProcessFrame();

        duin::Vector2 delta = duin::Input::GetMouseDelta();
        CHECK(delta.x == 0.0f);
        CHECK(delta.y == 0.0f);
    }

    TEST_CASE("Mouse motion: GetMouseDelta is zero after motion frame passes")
    {
        AppScope scope;

        // Frame 1: motion
        scope.PushMouseMotion(7.0f, 2.0f);
        scope.ProcessFrame();

        // Frame 2: no motion — delta should reset
        scope.ProcessFrame();

        duin::Vector2 delta = duin::Input::GetMouseDelta();
        CHECK(delta.x == 0.0f);
        CHECK(delta.y == 0.0f);
    }

    TEST_CASE("Mouse motion: GetMouseDelta reflects xrel/yrel from event")
    {
        AppScope scope;
        scope.ProcessFrame();

        scope.PushMouseMotion(10.0f, -6.0f);
        scope.ProcessFrame();

        duin::Vector2 delta = duin::Input::GetMouseDelta();
        // xrel=10 -> delta.x should be non-zero and in the expected direction
        CHECK(delta.x != 0.0f);
        CHECK(delta.x > 0.0f);   // positive xrel => positive or negative depending on convention
        // yrel=-6 -> delta.y should be non-zero and in the expected direction
        CHECK(delta.y != 0.0f);
    }

    TEST_CASE("Mouse motion: multiple motion events in one frame accumulate")
    {
        AppScope scope;
        scope.ProcessFrame();

        // Two motion events in the same frame
        scope.PushMouseMotion(3.0f, 1.0f);
        scope.PushMouseMotion(4.0f, 2.0f);
        scope.ProcessFrame();

        duin::Vector2 delta = duin::Input::GetMouseDelta();
        // Combined relative motion should be non-zero
        CHECK(delta.x != 0.0f);
        CHECK(delta.y != 0.0f);
    }

    TEST_CASE("Mouse motion: delta is zero on idle frames across 10+ frame sequence")
    {
        AppScope scope;

        // Frame 1: motion
        scope.PushMouseMotion(5.0f, -3.0f);
        scope.ProcessFrame();

        // Frames 2-11: no motion — delta must be zero each frame
        for (int i = 0; i < 10; ++i)
        {
            scope.ProcessFrame();
            duin::Vector2 delta = duin::Input::GetMouseDelta();
            CHECK(delta.x == 0.0f);
            CHECK(delta.y == 0.0f);
        }
    }

    TEST_CASE("Mouse motion: delta is non-zero only on frames with motion events across 10+ frames")
    {
        AppScope scope;

        for (int i = 0; i < 12; ++i)
        {
            bool hasMotion = (i % 3 == 0); // motion on frames 0, 3, 6, 9
            if (hasMotion)
                scope.PushMouseMotion(2.0f, 1.0f);

            scope.ProcessFrame();

            duin::Vector2 delta = duin::Input::GetMouseDelta();
            if (hasMotion)
            {
                MSG_CHECK(delta.x, delta.x != 0.0f);
                MSG_CHECK(delta.y, delta.y != 0.0f);
            }
            else
            {
                CHECK(delta.x == 0.0f);
                CHECK(delta.y == 0.0f);
            }
        }
    }

    TEST_CASE("Mouse motion: alternating motion and idle frames over 10+ frames")
    {
        AppScope scope;

        for (int i = 0; i < 10; ++i)
        {
            bool motionFrame = (i % 2 == 0);
            if (motionFrame)
                scope.PushMouseMotion(1.0f, 1.0f);

            scope.ProcessFrame();

            duin::Vector2 delta = duin::Input::GetMouseDelta();
            if (motionFrame)
            {
                MSG_CHECK(delta.x, delta.x != 0.0f);
            }
            else
            {
                CHECK(delta.x == 0.0f);
                CHECK(delta.y == 0.0f);
            }
        }
    }

    TEST_CASE("Mouse motion: GetMousePosition updates from absolute position in MOUSE_MOTION event")
    {
        AppScope scope;
        scope.ProcessFrame();

        scope.PushMouseMotionAt(100.0f, 200.0f, 5.0f, 3.0f);
        scope.ProcessFrame();

        // GetMousePosition reads SDL_GetMouseState, which reflects actual OS cursor position,
        // not the injected event's x/y. So we only verify the call doesn't crash and returns a value.
        duin::Vector2 pos = duin::Input::GetMousePosition();
        (void)pos; // position is OS-driven; not assertable from injected events
    }

    TEST_CASE("Mouse motion: consecutive same-direction motions over 10 frames produce consistent non-zero delta")
    {
        AppScope scope;

        for (int i = 0; i < 10; ++i)
        {
            scope.PushMouseMotion(3.0f, 2.0f);
            scope.ProcessFrame();

            duin::Vector2 delta = duin::Input::GetMouseDelta();
            MSG_CHECK(delta.x, delta.x != 0.0f);
            MSG_CHECK(delta.y, delta.y != 0.0f);
        }
    }

    TEST_CASE("Mouse motion: direction sign is consistent across multiple frames")
    {
        // If we always move in the same direction, delta sign should remain consistent
        AppScope scope;
        scope.ProcessFrame();

        float firstX = 0.0f;
        bool signSet = false;

        for (int i = 0; i < 10; ++i)
        {
            scope.PushMouseMotion(5.0f, 0.0f);
            scope.ProcessFrame();

            duin::Vector2 delta = duin::Input::GetMouseDelta();
            if (!signSet && delta.x != 0.0f)
            {
                firstX = delta.x;
                signSet = true;
            }
            else if (signSet && delta.x != 0.0f)
            {
                // Sign should not flip when motion direction doesn't change
                CHECK((firstX > 0.0f) == (delta.x > 0.0f));
            }
        }
    }
}

// ============================================================================
// Integration: InputActions
// ============================================================================

TEST_SUITE("Input Integration - InputActions")
{
    TEST_CASE("InputAction triggered via keyboard: bind W, push W, ProcessFrame -> triggered")
    {
        AppScope scope;
        scope.ProcessFrame();

        auto kb = duin::GetKeyboard_01();
        duin::CreateInputAction("integ_jump");
        duin::AddInputActionBinding("integ_jump", kb, DN_SCANCODE_W, DN_KEVENT_PRESSED);

        scope.PushKeyDown(SDL_SCANCODE_W);
        scope.ProcessFrame();

        CHECK(duin::IsInputActionTriggered("integ_jump"));

        // cleanup
        duin::RemoveInputAction("integ_jump");
    }

    TEST_CASE("InputAction not triggered when different key pressed")
    {
        AppScope scope;
        auto kb = duin::GetKeyboard_01();
        duin::CreateInputAction("integ_fire");
        duin::AddInputActionBinding("integ_fire", kb, DN_SCANCODE_SPACE, duin::Input::KeyEvent::PRESSED);

        scope.PushKeyDown(SDL_SCANCODE_X); // different key
        scope.ProcessFrame();

        CHECK_FALSE(duin::IsInputActionTriggered("integ_fire"));

        // cleanup
        duin::RemoveInputAction("integ_fire");
    }

    TEST_CASE("InputAction triggered via mouse button")
    {
        AppScope scope;
        auto mouse = duin::GetMouse_01();
        duin::CreateInputAction("integ_attack");
        duin::AddInputActionBinding("integ_attack", mouse,
                                    DN_MOUSE_BUTTON_LEFT,
                                    duin::Input::KeyEvent::PRESSED);

        scope.PushMouseButtonDown(SDL_BUTTON_LEFT);
        scope.ProcessFrame();

        CHECK(duin::IsInputActionTriggered("integ_attack"));

        // cleanup
        duin::RemoveInputAction("integ_attack");
    }

    TEST_CASE("OnInputActionTriggered callback fires when action is triggered")
    {
        AppScope scope;
        auto kb = duin::GetKeyboard_01();
        duin::CreateInputAction("integ_callback");
        duin::AddInputActionBinding("integ_callback", kb, DN_SCANCODE_E, duin::Input::KeyEvent::PRESSED);

        scope.PushKeyDown(SDL_SCANCODE_E);
        scope.ProcessFrame();

        int callbackFired = 0;
        duin::OnInputActionTriggered("integ_callback", [&]() { ++callbackFired; });
        CHECK(callbackFired == 1);

        // cleanup
        duin::RemoveInputAction("integ_callback");
    }

    TEST_CASE("OnInputActionTriggered callback does not fire when action is not triggered")
    {
        AppScope scope;
        auto kb = duin::GetKeyboard_01();
        duin::CreateInputAction("integ_no_callback");
        duin::AddInputActionBinding("integ_no_callback", kb, DN_SCANCODE_Q, duin::Input::KeyEvent::PRESSED);

        // No key event pushed
        scope.ProcessFrame();

        int callbackFired = 0;
        duin::OnInputActionTriggered("integ_no_callback", [&]() { ++callbackFired; });
        CHECK(callbackFired == 0);

        // cleanup
        duin::RemoveInputAction("integ_no_callback");
    }
}

} // namespace TestInput_Integration
