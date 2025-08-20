#pragma once

#include "Input.h"
#include "InputActions.h"

namespace duin {
    struct InputDevice_Mouse : public InputDevice
    {
        InputDevice_Mouse()
            : InputDevice() {}

        bool GetEvent(DN_Keycode key, Input::KeyEvent event) override
        {
            switch (event) {
                case Input::KeyEvent::HELD:
                    return Input::IsMouseButtonDown(key);
                    break;
                case Input::KeyEvent::IDLE:
                    return Input::IsMouseButtonUp(key);
                    break;
                case Input::KeyEvent::PRESSED:
                    return Input::IsMouseButtonPressed(key);
                    break;
                case Input::KeyEvent::PRESSED_REPEATED:
                    return Input::IsMouseButtonPressed(key);
                    break;
                case Input::KeyEvent::RELEASED:
                    return Input::IsMouseButtonReleased(key);
                    break;
                default:
                    return false;
                    break;
            }

            return false;
        };
    };
}
