#include "dnpch.h"
#include "EngineInput.h"
#include "Input.h"
#include "Duin/Core/Maths/DuinMaths.h" // for inputvector
#include "Duin/Core/Debug/DNLog.h"
#include "Duin/Core/Application.h"

#include <deque>

#include <external/backends/imgui_impl_sdl3.h>

#define EVENT_IS_KEYBOARD(event) (event >= 0x300 && event < 0x400)
#define EVENT_IS_MOUSE(event) (event >= 0x400 && event < 0x600)

namespace duin::Input
{
enum KeyState
{
    UP = 0,
    DOWN = 1
};
// enum KeyEvent {
//     PRESSED,
//     PRESSED_REPEATED,
//     HELD,
//     RELEASED,
//     IDLE
// };

// TODO move these to device struct, to allow multiple input devices
static const int MAX_KEYS = ::SDL_Scancode::SDL_SCANCODE_COUNT;
static KeyState previousKeyState[MAX_KEYS];
static KeyState currentKeyState[MAX_KEYS];

static const int MAX_MOUSE_KEYS = DN_MOUSE_BUTTON_CNT;
static KeyState previousMouseKeyState[MAX_MOUSE_KEYS];
static KeyState currentMouseKeyState[MAX_MOUSE_KEYS];

static DN_Keymod currentModifierState = DN_KEY_MOD_NONE;
static DN_Keymod previousModifierState = DN_KEY_MOD_NONE;

static Vector2 previousMouseLocalPos;
static Vector2 currentMouseLocalPos;
static Vector2 mouseFrameDelta;
static Vector2 mouseScrollDelta;
static float mouseDeltaX = 0.0f, mouseDeltaY = 0.0f;

void CacheCurrentKeyState()
{
    // Called in Application.cpp run
    memcpy(previousKeyState, currentKeyState, sizeof(previousKeyState));
}

void ClearCurrentKeyState()
{
    memset(currentKeyState, 0, sizeof(currentKeyState));
}

void CacheCurrentMouseKeyState()
{
    // Called in Application.cpp run
    memcpy(previousMouseKeyState, currentMouseKeyState, sizeof(previousMouseKeyState));
}

void ClearCurrentModKeyState()
{
    currentModifierState = DN_KEY_MOD_NONE;
}

void CacheCurrentModifierState()
{
    previousModifierState = currentModifierState;
}

void ClearCurrentMouseKeyState()
{
    memset(currentMouseKeyState, 0, sizeof(currentMouseKeyState));
}

void ClearCurrentMouseDelta()
{
    mouseScrollDelta = Vector2();
}

void StepInputStates()
{
    // Key states
    memset(previousKeyState, 0, sizeof(previousKeyState));                  // Clear old inputs
    memcpy(previousKeyState, currentKeyState, sizeof(previousKeyState));    // Cache current inputs
    memset(currentKeyState, 0, sizeof(currentKeyState));                    // Clear current inputs

    // Keymod states
    previousModifierState = DN_KEY_MOD_NONE;
    previousModifierState = currentModifierState;
    currentModifierState = DN_KEY_MOD_NONE;

    // Mouse key states
    memset(previousMouseKeyState, 0, sizeof(previousMouseKeyState));
    memcpy(previousMouseKeyState, currentMouseKeyState, sizeof(previousMouseKeyState));
    memset(currentMouseKeyState, 0, sizeof(currentMouseKeyState));

    // Mouse scroll delta
    mouseScrollDelta = Vector2();

    // Mouse position state
    previousMouseLocalPos = Vector2();
    previousMouseLocalPos = currentMouseLocalPos;
    mouseDeltaX = 0.0f;
    mouseDeltaY = 0.0f;
}

void ResetMouseFrameState()
{
    previousMouseLocalPos = currentMouseLocalPos;
    mouseDeltaX = 0.0f;
    mouseDeltaY = 0.0f;
    mouseScrollDelta = Vector2();
}

void ResetAllInputState()
{
    memset(previousKeyState, 0, sizeof(previousKeyState));
    memset(currentKeyState, 0, sizeof(currentKeyState));
    memset(previousMouseKeyState, 0, sizeof(previousMouseKeyState));
    memset(currentMouseKeyState, 0, sizeof(currentMouseKeyState));
    currentModifierState = DN_KEY_MOD_NONE;
    previousModifierState = DN_KEY_MOD_NONE;
    previousMouseLocalPos = Vector2();
    currentMouseLocalPos = Vector2();
    mouseFrameDelta = Vector2();
    mouseScrollDelta = Vector2();
    mouseDeltaX = 0.0f;
    mouseDeltaY = 0.0f;
}

/**
 * @brief Test the SDL event to see if it is a keyboard event, then process the event. This function is called from
 * Application.Run().
 * @param e
 */
void ProcessSDLKeyboardEvent(::SDL_Event e)
{
    if (!EVENT_IS_KEYBOARD(e.type))
    {
        return;
    }
    // Set current key state
    ::SDL_Scancode code = e.key.scancode;
    if (code > 511 || code < 0)
    {
        DN_CORE_WARN("Keyboard event out of bounds {}!", (int)code);
        return;
    }

    KeyState state = KeyState::UP;
    if (e.type == SDL_EVENT_KEY_DOWN)
    {
        state = KeyState::DOWN;
    }
    if (e.type == SDL_EVENT_KEY_UP)
    {
        state = KeyState::UP;
    }
    currentKeyState[code] = state;
}

int IsKeyPressed(DN_Scancode code)
{
    return (currentKeyState[code] && !previousKeyState[code]);
}

int IsKeyPressedAgain(DN_Scancode code)
{
    // TODO

    return 0;
}

int IsKeyReleased(DN_Scancode code)
{
    return (!currentKeyState[code] && previousKeyState[code]);
}

int IsKeyDown(DN_Scancode code)
{
    return currentKeyState[code];
}

int IsKeyUp(DN_Scancode code)
{
    return !currentKeyState[code];
}

int IsInputVectorPressed(DN_Scancode up, DN_Scancode down, DN_Scancode left, DN_Scancode right)
{
    return (IsKeyDown(up) || IsKeyDown(down) || IsKeyDown(left) || IsKeyDown(right));
}

Vector2 GetInputVector(DN_Scancode up, DN_Scancode down, DN_Scancode left, DN_Scancode right)
{
    return Vector2NormalizeF(
        Vector2((float)(IsKeyDown(left) - IsKeyDown(right)), (float)(IsKeyDown(down) - IsKeyDown(up))));
}

DN_Scancode GetKeyPressed()
{
    // TODO
    return DN_SCANCODE_UNKNOWN;
}

// Expand a modifier mask: replace each combined alias (CTRL, SHIFT, ALT, GUI) with
// any of its individual components that are present in `active`.
// This allows DN_KEY_MOD_CTRL to match when either LCTRL or RCTRL is active,
// while keeping LCTRL and RCTRL bindings side-specific.
static DN_Keymod ExpandBinding(DN_Keymod binding, DN_Keymod active)
{
    DN_Keymod expanded = binding;
    // For each combined pair: if the binding requests the combined bit but not a specific
    // side, replace the combined bits with whichever individual bits are active.
    auto expandPair = [&](DN_Keymod combined, DN_Keymod left, DN_Keymod right) {
        DN_Keymod both = left | right;
        if ((binding & both) == both) {
            // Binding uses the combined alias (both bits set) — satisfied by any active side
            if (active & left)  expanded = (expanded & ~both) | left;
            else if (active & right) expanded = (expanded & ~both) | right;
        }
    };
    expandPair(DN_KEY_MOD_CTRL,  DN_KEY_MOD_LCTRL,  DN_KEY_MOD_RCTRL);
    expandPair(DN_KEY_MOD_SHIFT, DN_KEY_MOD_LSHIFT, DN_KEY_MOD_RSHIFT);
    expandPair(DN_KEY_MOD_ALT,   DN_KEY_MOD_LALT,   DN_KEY_MOD_RALT);
    expandPair(DN_KEY_MOD_GUI,   DN_KEY_MOD_LGUI,   DN_KEY_MOD_RGUI);
    return expanded;
}

int IsModifierDown(DN_Keymod code)
{
    DN_Keymod active = (DN_Keymod)::SDL_GetModState();
    DN_Keymod binding = ExpandBinding(code, active);
    return (active & binding) == binding;
}

int IsModifierExact(DN_Keymod code)
{
    return (DN_Keymod)::SDL_GetModState() == code;
}

int IsModifierDown(DN_Scancode code)
{
    DN_Keymod mod = DN_KEY_MOD_NONE;
    switch (code)
    {
    case DN_SCANCODE_LCTRL:  mod = DN_KEY_MOD_LCTRL;  break;
    case DN_SCANCODE_RCTRL:  mod = DN_KEY_MOD_RCTRL;  break;
    case DN_SCANCODE_LSHIFT: mod = DN_KEY_MOD_LSHIFT; break;
    case DN_SCANCODE_RSHIFT: mod = DN_KEY_MOD_RSHIFT; break;
    case DN_SCANCODE_LALT:   mod = DN_KEY_MOD_LALT;   break;
    case DN_SCANCODE_RALT:   mod = DN_KEY_MOD_RALT;   break;
    case DN_SCANCODE_LGUI:   mod = DN_KEY_MOD_LGUI;   break;
    case DN_SCANCODE_RGUI:   mod = DN_KEY_MOD_RGUI;   break;
    default: return 0;
    }
    return (currentModifierState & mod) != 0;
}

/**
 * @brief Test the SDL event to see if it is a mouse event, then process the event. This function is called from
 * Application.Run().
 * @param e
 */
void ProcessSDLMouseEvent(::SDL_Event e)
{
    if (e.type == SDL_EVENT_MOUSE_MOTION)
    {
        float x = e.motion.x;
        float y = e.motion.y;
        currentMouseLocalPos = Vector2(x, y);

        mouseDeltaX += e.motion.xrel;
        mouseDeltaY += e.motion.yrel;
    }

    KeyState state = KeyState::UP;
    if (e.type == SDL_EVENT_MOUSE_BUTTON_DOWN)
    {
        state = KeyState::DOWN;
        DN_MouseButtonFlags btnIdx = e.button.button - 1;
        currentMouseKeyState[btnIdx] = state;
    }
    if (e.type == SDL_EVENT_MOUSE_BUTTON_UP)
    {
        state = KeyState::UP;
        DN_MouseButtonFlags btnIdx = e.button.button - 1;
        currentMouseKeyState[btnIdx] = state;
    }

    if (e.type == SDL_EVENT_MOUSE_WHEEL)
    {
        DN_CORE_INFO("MOUSE_WHEEL {} {}", e.wheel.x, e.wheel.y);
        mouseScrollDelta.x += e.wheel.x;
        mouseScrollDelta.y += e.wheel.y;
    }


}

void UpdateMouseFrameDelta()
{
    mouseFrameDelta = Vector2(mouseDeltaX, mouseDeltaY);
}

void CaptureMouse(int enable)
{
    ::SDL_SetWindowRelativeMouseMode(GetSDLWindow(), enable);
}

int IsMouseButtonPressed(DN_MouseButtonFlags button)
{
    return (currentMouseKeyState[button - 1] && !previousMouseKeyState[button - 1]);
}

int IsMouseButtonDown(DN_MouseButtonFlags button)
{
    return currentMouseKeyState[button - 1];
}

int IsMouseButtonReleased(DN_MouseButtonFlags button)
{
    return (!currentMouseKeyState[button - 1] && previousMouseKeyState[button - 1]);
}

int IsMouseButtonUp(DN_MouseButtonFlags button)
{
    return !currentMouseKeyState[button - 1];
}

Vector2 GetMouseGlobalPosition(void)
{
    float x = 0, y = 0;
    ::SDL_GetGlobalMouseState(&x, &y);

    return Vector2(x, y);
}

Vector2 GetMousePosition(void)
{
    return currentMouseLocalPos;
}

Vector2 GetMouseDelta(void)
{
    return mouseFrameDelta;
}

void SetMousePosition(int x, int y)
{
}

void SetMouseOffset(int offsetX, int offsetY)
{
}

void SetMouseScale(float scaleX, float scaleY)
{
}

float GetMouseWheelMove(void)
{

    return mouseScrollDelta.y;
}

Vector2 GetMouseWheelMoveV(void)
{
    return mouseScrollDelta;
}

void SetMouseCursor(int cursor)
{
}

} // namespace duin::Input
