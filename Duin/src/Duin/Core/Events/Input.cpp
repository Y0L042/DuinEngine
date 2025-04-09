#include "dnpch.h"
#include "EngineInput.h"
#include "Input.h"
#include "Duin/Core/Maths/DuinMaths.h" // for inputvector
#include "Duin/Core/Debug/DNLog.h"

#include <deque>

#include <external/backends/imgui_impl_sdl3.h>

#define EVENT_IS_KEYBOARD(event) (event >= 0x300 && event < 0x400)
#define EVENT_IS_MOUSE(event) (event >= 0x400 && event < 0x600)

namespace duin::Input {
    enum KeyState{
        UP = 0,
        DOWN = 1
    };
    enum KeyEvent {
        PRESSED,
        PRESSED_REPEATED,
        HELD,
        RELEASED,
        IDLE
    };

    static const int MAX_KEYS = ::SDL_Scancode::SDL_SCANCODE_COUNT;
    KeyState previousKeyState[MAX_KEYS]; 
    KeyState currentKeyState[MAX_KEYS]; 

    struct KeyInputEvent {
        DN_Keycode keyCode;
        DN_Keymod keyMod;
        KeyState keyState;
        KeyEvent keyEvent;
        ::SDL_Event sdlEvent;
    };
    std::deque<KeyInputEvent> inputBuffer;

    KeyInputEvent ProcessSDLKeyboardEvent(::SDL_Event e);

    void CacheCurrentKeyState()
    {
        memcpy(previousKeyState, currentKeyState, sizeof(previousKeyState));
    }

    void ClearCurrentKeyState()
    {
        memset(currentKeyState, 0, sizeof(currentKeyState));
        inputBuffer.clear();
    }

    KeyInputEvent ProcessSDLKeyboardEvent(::SDL_Event e)
    {
        // Set current key state
        ::SDL_Scancode code = e.key.scancode;
        KeyState state = e.key.down ? KeyState::DOWN : KeyState::UP;
        currentKeyState[code] = state;

        // Create KeyEvent
        KeyState previousState = previousKeyState[code];
        KeyEvent keyEvent;
        if (e.key.repeat) {
            keyEvent = KeyEvent::PRESSED_REPEATED;
        } else {
            if (state == KeyState::DOWN) {
                if (previousState == KeyState::DOWN) {
                    keyEvent = KeyEvent::HELD;
                } else /* previousState == KeyState::UP */ {
                    keyEvent = KeyEvent::PRESSED;
                }
            } else /* state == KeyState::UP */{
                if (previousState == KeyState::DOWN) {
                    keyEvent = KeyEvent::RELEASED;
                } else /* previousState == KeyState::UP */ {
                    keyEvent = KeyEvent::IDLE; 
                }
            }
        }

        KeyInputEvent inputEvent;
        inputEvent.keyCode = e.key.key;
        inputEvent.keyMod = e.key.mod;
        inputEvent.keyState = state;
        inputEvent.keyEvent = keyEvent; 
        inputEvent.sdlEvent = e;

		return inputEvent;
    }

    void EngineInput_GetEvent(::SDL_Event e)
    {
        if (EVENT_IS_KEYBOARD(e.type)) {
            KeyInputEvent inputEvent = ProcessSDLKeyboardEvent(e);
            // inputBuffer.push_back(inputEvent);
        }
    }

    int IsKeyPressed(DN_Keycode code)
    {
        // Review modstate param
        ::SDL_Scancode scanCode = ::SDL_GetScancodeFromKey(code, NULL);
        return (currentKeyState[scanCode] && !previousKeyState[scanCode]);
    }

    int IsKeyPressedAgain(DN_Keycode code)
	{
        // TODO

        return 0;
	}

    int IsKeyReleased(DN_Keycode code)
	{
        // Review modstate param
        ::SDL_Scancode scanCode = ::SDL_GetScancodeFromKey(code, NULL);
        return (!currentKeyState[scanCode] && previousKeyState[scanCode]);
	}
     
    int IsKeyDown(DN_Keycode code)
	{
        // Review modstate param
        ::SDL_Scancode scanCode = ::SDL_GetScancodeFromKey(code, NULL);
        return currentKeyState[scanCode];
	}
             
    int IsKeyUp(DN_Keycode code)
	{
        // Review modstate param
        ::SDL_Scancode scanCode = ::SDL_GetScancodeFromKey(code, NULL);
        return !currentKeyState[scanCode];
	}

    int IsInputVectorPressed(DN_Keycode up, DN_Keycode down, DN_Keycode left, DN_Keycode right)
    {
        return IsKeyDown(up) || IsKeyDown(down) || IsKeyDown(left) || IsKeyDown(right);
    }

    Vector2 GetInputVector(DN_Keycode up, DN_Keycode down, DN_Keycode left, DN_Keycode right)
    {
        return Vector2(-IsKeyDown(left) + IsKeyDown(right), IsKeyDown(up) - IsKeyDown(down));
    }
               
         
    DN_Keycode GetKeyPressed()
	{
		return 0;
	}
                 
}
