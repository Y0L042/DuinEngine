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

    bool GetEvent(DN_InputCode key, Input::KeyEvent event, DN_InputCode modifier = 0) override
    {
        DN_MouseButtonFlags button = static_cast<DN_MouseButtonFlags>(key);
        switch (event)
        {
        case Input::KeyEvent::HELD:
            return Input::IsMouseButtonDown(button);
        case Input::KeyEvent::IDLE:
            return Input::IsMouseButtonUp(button);
        case Input::KeyEvent::PRESSED:
            return Input::IsMouseButtonPressed(button);
        case Input::KeyEvent::PRESSED_REPEATED:
            return Input::IsMouseButtonPressed(button);
        case Input::KeyEvent::RELEASED:
            return Input::IsMouseButtonReleased(button);
        default:
            return false;
        }
    }
};

} // namespace duin
