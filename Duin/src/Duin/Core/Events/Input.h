/**
 * @file Input.h
 * @brief Low-level input query functions.
 * @ingroup Core_Events
 *
 * Provides direct keyboard and mouse state queries. For higher-level input
 * handling, consider using the InputActions system instead.
 */

#pragma once

#include "Keys.h"

/** @name Key Event Macros
 * Shorthand for key event types.
 * @{
 */
#define DN_KEVENT_PRESSED duin::Input::KeyEvent::PRESSED
#define DN_KEVENT_PRESSED_REPEATED duin::Input::KeyEvent::PRESSED_REPEATED
#define DN_KEVENT_HELD duin::Input::KeyEvent::HELD
#define DN_KEVENT_RELEASED duin::Input::KeyEvent::RELEASED
#define DN_KEVENT_IDLE duin::Input::KeyEvent::IDLE
/** @} */

namespace duin
{
struct Vector2;
}

/**
 * @namespace duin::Input
 * @brief Low-level input state queries.
 * @ingroup Core_Events
 */
namespace duin::Input
{

/**
 * @enum KeyEvent
 * @brief Types of key/button events.
 */
enum KeyEvent
{
    PRESSED,          ///< Key was just pressed this frame.
    PRESSED_REPEATED, ///< Key is repeating (held down, OS repeat).
    HELD,             ///< Key is currently held down.
    RELEASED,         ///< Key was just released this frame.
    IDLE              ///< Key is not pressed.
};

/**
 * @name Keyboard Functions
 * @{
 */

/** @brief Checks if key was pressed this frame (not held). */
int IsKeyPressed(DN_Keycode code);
/** @brief Checks if key was pressed again (repeat). */
int IsKeyPressedAgain(DN_Keycode code);
/** @brief Checks if key is currently held down. */
int IsKeyDown(DN_Keycode code);
/** @brief Checks if key is not held down. */
int IsKeyUp(DN_Keycode code);
/** @brief Checks if key was released this frame. */
int IsKeyReleased(DN_Keycode code);
/** @brief Checks if any directional key is pressed. */
int IsInputVectorPressed(DN_Keycode up, DN_Keycode down, DN_Keycode left, DN_Keycode right);
/** @brief Returns normalized direction vector from WASD-style keys. */
Vector2 GetInputVector(DN_Keycode up, DN_Keycode down, DN_Keycode left, DN_Keycode right);
/** @brief Returns next pressed key code (0 when queue empty). */
DN_Keycode GetKeyPressed();
/** @} */

/**
 * @name Mouse Functions
 * @{
 */

/** @brief Locks mouse cursor to window. */
void CaptureMouse(int enable);
/** @brief Checks if mouse button was pressed this frame. */
int IsMouseButtonPressed(DN_MouseButtonFlags button);
/** @brief Checks if mouse button is held down. */
int IsMouseButtonDown(DN_MouseButtonFlags button);
/** @brief Checks if mouse button was released this frame. */
int IsMouseButtonReleased(DN_MouseButtonFlags button);
/** @brief Checks if mouse button is not pressed. */
int IsMouseButtonUp(DN_MouseButtonFlags button);
/** @brief Returns mouse position relative to screen. */
Vector2 GetMouseGlobalPosition(void);
/** @brief Returns mouse position relative to game window. */
Vector2 GetMousePosition(void);
/** @brief Returns mouse movement since last frame. */
Vector2 GetMouseDelta(void);
/** @brief Sets mouse cursor position. */
void SetMousePosition(int x, int y);
/** @brief Sets mouse position offset for calculations. */
void SetMouseOffset(int offsetX, int offsetY);
/** @brief Sets mouse position scaling. */
void SetMouseScale(float scaleX, float scaleY);
/** @brief Returns scroll wheel movement (max of X or Y). */
float GetMouseWheelMove(void);
/** @brief Returns scroll wheel movement as 2D vector. */
Vector2 GetMouseWheelMoveV(void);
/** @brief Sets the mouse cursor style. */
void SetMouseCursor(int cursor);
/** @} */

} // namespace duin::Input
