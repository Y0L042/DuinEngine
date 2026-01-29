/**
 * @file InputDevice_Keyboard.h
 * @brief Keyboard input device implementation.
 * @ingroup Core_Events
 */

#pragma once

#include "Input.h"
#include "InputActions.h"

namespace duin
{

/**
 * @struct InputDevice_Keyboard
 * @brief Keyboard implementation of InputDevice.
 * @ingroup Core_Events
 *
 * Translates InputAction queries into keyboard state checks.
 * Use with InputBinding to bind actions to keyboard keys.
 */
struct InputDevice_Keyboard : public InputDevice
{
    InputDevice_Keyboard() : InputDevice()
    {
    }

    bool GetEvent(DN_Keycode key, Input::KeyEvent event) override
    {
        switch (event)
        {
        case Input::KeyEvent::HELD:
            return Input::IsKeyDown(key);
        case Input::KeyEvent::IDLE:
            return Input::IsKeyUp(key);
        case Input::KeyEvent::PRESSED:
            return Input::IsKeyPressed(key);
        case Input::KeyEvent::PRESSED_REPEATED:
            return Input::IsKeyPressedAgain(key);
        case Input::KeyEvent::RELEASED:
            return Input::IsKeyReleased(key);
        default:
            return false;
        }
    }
};

} // namespace duin
