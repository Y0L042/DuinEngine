#pragma once

// TODO add raylib license

#include "Keys.h"

namespace duin {
    struct Vector2;
}

namespace duin::Input {
    /* KEYBOARD */
    int IsKeyPressed(DN_Keycode code);          // Down and up, not held
    int IsKeyPressedAgain(DN_Keycode code);     // Pressed more than once in short period
    int IsKeyDown(DN_Keycode code);             // Is key held down
    int IsKeyUp(DN_Keycode code);               // Is key not held down
    int IsKeyReleased(DN_Keycode code);         // Is key released after being pressed
    int IsInputVectorPressed(DN_Keycode up, DN_Keycode down, DN_Keycode left, DN_Keycode right);
    Vector2 GetInputVector(DN_Keycode up, DN_Keycode down, DN_Keycode left, DN_Keycode right);
    DN_Keycode GetKeyPressed();                 // Returns key that is pressed. Call multiple times for all keys pressed. Empty when it returns 0
    // TODO void SetExitKey(DN_Keycode code);



    /* MOUSE */
	void CaptureMouse(int enable);                               // Capture mouse (lock mouse cursor in window)

    int IsMouseButtonPressed(int button);                  // Check if a mouse button has been pressed once
    int IsMouseButtonDown(int button);                     // Check if a mouse button is being pressed
    int IsMouseButtonReleased(int button);                 // Check if a mouse button has been released once
    int IsMouseButtonUp(int button);                       // Check if a mouse button is NOT being pressed
    Vector2 GetMouseGlobalPosition(void);                         // Get mouse position XY relative to system window 
    Vector2 GetMousePosition(void);                         // Get mouse position XY relative to game window
    Vector2 GetMouseDelta(void);                            // Get mouse delta between frames
    void SetMousePosition(int x, int y);                    // Set mouse position XY
    void SetMouseOffset(int offsetX, int offsetY);          // Set mouse offset
    void SetMouseScale(float scaleX, float scaleY);         // Set mouse scaling
    float GetMouseWheelMove(void);                          // Get mouse wheel movement for X or Y, whichever is larger
    Vector2 GetMouseWheelMoveV(void);                       // Get mouse wheel movement for both X and Y
    void SetMouseCursor(int cursor);                        // Set mouse cursor
}
