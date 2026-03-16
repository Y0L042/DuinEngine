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

    bool GetEvent(DN_InputCode key, Input::KeyEvent event, DN_InputCode modifier) override
    {
        bool keyInput = false;
        bool modInput = false;
        DN_Scancode scancode = static_cast<DN_Scancode>(key);

        switch (event)
        {
        case Input::KeyEvent::HELD:
            keyInput = Input::IsKeyDown(scancode);
            break;
        case Input::KeyEvent::IDLE:
            keyInput = Input::IsKeyUp(scancode);
            break;
        case Input::KeyEvent::PRESSED:
            keyInput = Input::IsKeyPressed(scancode);
            break;
        case Input::KeyEvent::PRESSED_REPEATED:
            keyInput = Input::IsKeyPressedAgain(scancode);
            break;
        case Input::KeyEvent::RELEASED:
            keyInput = Input::IsKeyReleased(scancode);
            break;
        default:
            keyInput = false;
            break;
        }

        if (modifier)
        {
            modInput = Input::IsModifierDown(static_cast<DN_Keymod>(modifier));
            return keyInput && modInput;
        }

        return keyInput;
    }
};

} // namespace duin
