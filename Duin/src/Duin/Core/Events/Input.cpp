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
    static KeyState previousKeyState[MAX_KEYS]; 
    static KeyState currentKeyState[MAX_KEYS]; 

    static Vector2 previousMouseLocalPos;
    static Vector2 currentMouseLocalPos;
    static Vector2 mouseFrameDelta;

    void CacheCurrentKeyState()
    {
        memcpy(previousKeyState, currentKeyState, sizeof(previousKeyState));
    }

    void ClearCurrentKeyState()
    {
        memset(currentKeyState, 0, sizeof(currentKeyState));
    }

    void ProcessSDLKeyboardEvent(::SDL_Event e)
    {
        if (!EVENT_IS_KEYBOARD(e.type)) { return; }
        // Set current key state
        ::SDL_Scancode code = e.key.scancode;
        KeyState state;
        if (e.type == SDL_EVENT_KEY_DOWN) { state = KeyState::DOWN; }
        if (e.type == SDL_EVENT_KEY_UP) { state = KeyState::UP; }
        currentKeyState[code] = state;
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
        return Vector2NormalizeF(Vector2(IsKeyDown(left) - IsKeyDown(right), IsKeyDown(down) - IsKeyDown(up)));
    }
               
         
    DN_Keycode GetKeyPressed()
	{
        // TODO
		return 0;
	}
                 

    void ProcessSDLMouseEvent(::SDL_Event e)
    {
        previousMouseLocalPos = currentMouseLocalPos;

        if (e.type == SDL_EVENT_MOUSE_MOTION) {
            float x, y = 0;
            ::SDL_GetMouseState(&x, &y);
            currentMouseLocalPos = Vector2(x, y);
        }
        if (e.type == SDL_EVENT_MOUSE_BUTTON_DOWN) {

        }
    }

    void UpdateMouseFrameDelta()
    {
        float x, y = 0;
        ::SDL_GetRelativeMouseState(&x, &y);
		mouseFrameDelta = Vector2(-x, y); // Invert x-axis
    }

    void CaptureMouse(int enable)
    {
        ::SDL_SetWindowRelativeMouseMode(GetSDLWindow(), enable);
    }

    int IsMouseButtonPressed(int button)
    {
        return 0;
    }

    int IsMouseButtonDown(int button)
	{
        return 0;
	}
                     
    int IsMouseButtonReleased(int button)
	{
        return 0;
	}
                 
    int IsMouseButtonUp(int button)
	{
        return 0;
	}
                       
    Vector2 GetMouseGlobalPosition(void)
	{
        float x, y = 0;
        ::SDL_GetGlobalMouseState(&x, &y);

        return Vector2(x, y);
	}

    Vector2 GetMousePosition(void)
	{
        float x, y = 0;
        ::SDL_GetMouseState(&x, &y);

        return Vector2(x, y);
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
        return 0.0f;
	}
                         
    Vector2 GetMouseWheelMoveV(void)
	{
        return Vector2();
	}
                      
    void SetMouseCursor(int cursor)
	{
	
	}
                       
}
