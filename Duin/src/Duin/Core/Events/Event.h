/**
 * @file Event.h
 * @brief Event wrapper classes for SDL events.
 * @ingroup Core_Events
 */

#pragma once

#include <SDL3/SDL_events.h>

namespace duin
{

/** @brief Key code type (SDL scancode). */
typedef Uint32 DN_Keycode;

/**
 * @class Event
 * @brief Base wrapper for SDL events.
 * @ingroup Core_Events
 *
 * Wraps an SDL_Event for use in the engine's event system.
 * Derived classes provide type-specific query methods.
 */
class Event
{
  public:
    ::SDL_Event sdlEvent; ///< The underlying SDL event data.

  protected:
    friend class EventHandler;
    void SetSDLEvent(::SDL_Event e);
};

/**
 * @class InputEvent
 * @brief Event class for keyboard and mouse input.
 * @ingroup Core_Events
 */
class InputEvent : public Event
{
  public:
    InputEvent() = default;
    ~InputEvent() = default;

    /**
     * @brief Checks if a key is currently pressed.
     * @param key The key code to check.
     * @return Non-zero if key is down.
     */
    int IsKeyDown(DN_Keycode key);

    /** @brief Checks if any mouse button is pressed. @return Non-zero if pressed. */
    int IsMouseButtonPressed();
};

/**
 * @class WindowEvent
 * @brief Event class for window-related events (resize, focus, etc.).
 * @ingroup Core_Events
 */
class WindowEvent : public Event
{
  public:
    WindowEvent() = default;
    ~WindowEvent() = default;
};

} // namespace duin
