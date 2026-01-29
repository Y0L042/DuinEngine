/**
 * @file EventHandler.h
 * @brief Event polling and dispatch system.
 * @ingroup Core_Events
 */

#pragma once

#include <vector>
#include <functional>

#include "Event.h"

namespace duin
{

/**
 * @class EventHandler
 * @brief Singleton that polls SDL events and dispatches them to listeners.
 * @ingroup Core_Events
 *
 * EventHandler wraps SDL's event system. It polls events each frame and
 * invokes registered listener callbacks. The engine uses this internally;
 * user code typically handles events through Application::OnEvent() or
 * the InputActions system instead.
 */
class EventHandler
{
  public:
    /** @brief Returns the singleton instance. */
    static EventHandler &Get();

    /** @brief Called by engine to process a polled SDL event. */
    static void GetPolledEvent(::SDL_Event e);

    EventHandler();
    ~EventHandler();

    /** @brief Returns true if a close/quit event was received. */
    bool IsCloseRequested();

    /**
     * @brief Registers a callback for input events.
     * @param listener Function to call when input events occur.
     */
    void RegisterInputEventListener(std::function<void(Event)> listener);

  private:
    int closeRequested;
    std::vector<std::function<void(Event)>> inputEventListeners;

    void CallInputEventListeners(Event event);
};

} // namespace duin
