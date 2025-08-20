#pragma once

#include "Input.h"
#include "InputActions.h"

namespace duin {
    struct InputDevice_Keyboard : public InputDevice
    {
        InputDevice_Keyboard()
            : InputDevice() {}

        bool GetEvent(DN_Keycode key, Input::KeyEvent event) override
        {
            switch (event) {
                case Input::KeyEvent::HELD:
                    return Input::IsKeyDown(key);
                    break;
                case Input::KeyEvent::IDLE:
                    return Input::IsKeyUp(key);
                    break;
                case Input::KeyEvent::PRESSED:
                    return Input::IsKeyPressed(key);
                    break;
                case Input::KeyEvent::PRESSED_REPEATED:
                    return Input::IsKeyPressedAgain(key);
                    break;
                case Input::KeyEvent::RELEASED:
                    return Input::IsKeyReleased(key);
                    break;
                default:
                    return false;
                    break;
            }

            return false;
        };
    };
}
