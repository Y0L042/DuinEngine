/**
 * @file InputDevice_Mouse.h
 * @brief Mouse input device implementation.
 * @ingroup Core_Events
 */

#pragma once

#include "Input.h"
#include "InputActions.h"

namespace duin
{

/**
 * @struct InputDevice_Mouse
 * @brief Mouse implementation of InputDevice.
 * @ingroup Core_Events
 *
 * Translates InputAction queries into mouse button state checks.
 * Use with InputBinding to bind actions to mouse buttons.
 */
struct InputDevice_Mouse : public InputDevice
{
    InputDevice_Mouse() : InputDevice()
    {
    }

    bool GetEvent(DN_Keycode key, Input::KeyEvent event) override
    {
        switch (event)
        {
        case Input::KeyEvent::HELD:
            return Input::IsMouseButtonDown(key);
        case Input::KeyEvent::IDLE:
            return Input::IsMouseButtonUp(key);
        case Input::KeyEvent::PRESSED:
            return Input::IsMouseButtonPressed(key);
        case Input::KeyEvent::PRESSED_REPEATED:
            return Input::IsMouseButtonPressed(key);
        case Input::KeyEvent::RELEASED:
            return Input::IsMouseButtonReleased(key);
        default:
            return false;
        }
    }
};

} // namespace duin
