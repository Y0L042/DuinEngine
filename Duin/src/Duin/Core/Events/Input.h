#pragma once

#include "Keys.h"
#include "Duin/Core/Maths/DuinMaths.h" // for inputvector

namespace duin::Input {

    int IsKeyPressed(DN_Keycode code);          // Down and up, not held
    int IsKeyPressedAgain(DN_Keycode code);     // Pressed more than once in short period
    int IsKeyDown(DN_Keycode code);             // Is key held down
    int IsKeyUp(DN_Keycode code);               // Is key not held down
    int IsKeyReleased(DN_Keycode code);         // Is key released after being pressed

    int IsInputVectorPressed(DN_Keycode up, DN_Keycode down, DN_Keycode left, DN_Keycode right);
    Vector2 GetInputVector(DN_Keycode up, DN_Keycode down, DN_Keycode left, DN_Keycode right);

    DN_Keycode GetKeyPressed();                 // Returns key that is pressed. Call multiple times for all keys pressed. Empty when it returns 0
    // TODO void SetExitKey(DN_Keycode code);
}
