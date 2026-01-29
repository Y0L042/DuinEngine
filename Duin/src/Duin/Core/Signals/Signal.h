#pragma once

#include "Duin/Core/Utils/UUID.h"

#include <vector>
#include <functional>
#include <utility>

namespace duin
{

/**
 * @brief Signal class for event-driven programming.
 *
 * A type-safe signal/slot system supporting multiple listeners.
 * Listeners are identified by UUIDs and can be connected, disconnected, and notified with arbitrary arguments.
 *
 * @tparam types Argument types passed to listeners when the signal is emitted.
 *
 * @code
 * duin::Signal<int, std::string> sig;
 * duin::UUID id = sig.Connect([](int a, const std::string& b) { ... });
 * sig.Emit(42, "hello");
 * sig.Disconnect(id);
 * @endcode
 */
template <typename... types>
class Signal
{
  public:
    /**
     * @brief Listener type: a pair of UUID and callback function.
     */
    using Listener = std::pair<UUID, std::function<void(types...)>>;

    /**
     * @brief Connects a listener to the signal.
     *
     * Registers a callback to be invoked when the signal is emitted.
     * Returns a unique UUID for the listener, which can be used to disconnect it later.
     *
     * @param callback The function to call when the signal is emitted.
     * @return UUID assigned to the listener.
     */
    UUID Connect(std::function<void(types...)> callback)
    {
        UUID uuid;
        listeners.emplace_back(Listener(uuid, callback));

        return uuid;
    }

    /**
     * @brief Disconnects a listener by UUID.
     *
     * Removes the listener associated with the given UUID.
     *
     * @param uuid The UUID of the listener to disconnect.
     * @return true if a listener was removed, false otherwise.
     */
    bool Disconnect(UUID uuid)
    {
        for (int i = 0; i < listeners.size(); ++i)
        {
            if (listeners[i].first == uuid)
            {
                std::swap(listeners.back(), listeners.at(i));
                listeners.pop_back();

                return true;
            }
        }

        return false;
    }

    /**
     * @brief Emits the signal to all connected listeners.
     *
     * Invokes all registered callbacks with the provided arguments.
     *
     * @param args Arguments to pass to each listener.
     */
    void Emit(types... args)
    {
        for (Listener &listener : listeners)
        {
            if (listener.second)
            {
                listener.second(args...);
            }
        }
    }

    /**
     * @brief Gets the UUID of this Signal instance.
     *
     * @return UUID of the signal.
     */
    UUID GetUUID()
    {
        return uuid;
    }

    /**
     * @brief Returns count of connected listeners.
     *
     * @return Int count of listeners.
     */
    int GetListenerCount()
    {
        return listeners.size();
    }

  private:
    UUID uuid;                       ///< Unique identifier for this Signal instance.
    std::vector<Listener> listeners; ///< List of connected listeners.
};
} // namespace duin
