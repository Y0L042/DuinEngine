/**
 * @file InputActions.h
 * @brief Action-based input system.
 * @ingroup Core_Events
 *
 * Provides a layer of abstraction over raw input. Named actions can be
 * bound to multiple input sources (keyboard keys, mouse buttons) and
 * queried by name rather than device-specific codes.
 *
 * @code
 * // Setup
 * duin::CreateInputAction("jump");
 * duin::AddInputActionBinding("jump", keyboard, DN_KEY_SPACE, Input::KeyEvent::KEY_PRESSED);
 *
 * // Query
 * if (duin::IsInputActionTriggered("jump")) {
 *     // Handle jump
 * }
 * @endcode
 */

#pragma once

#include "Input.h"
#include "Duin/Core/Utils/UUID.h"

#include <vector>
#include <string>
#include <memory>

namespace duin
{

/**
 * @struct InputDevice
 * @brief Base interface for input devices.
 * @ingroup Core_Events
 *
 * Represents an input source (keyboard, mouse, gamepad). Derived classes
 * implement GetEvent() to report device state.
 */
struct InputDevice
{
    UUID uuid; ///< Unique identifier for this device instance.

    /**
     * @brief Queries the device for a specific input state.
     * @param key The key/button code.
     * @param event The event type to check (pressed, released, held).
     * @return True if the condition is met.
     */
    virtual bool GetEvent(DN_Keycode key, Input::KeyEvent event)
    {
        return false;
    }
};

/**
 * @struct InputBinding
 * @brief Maps a device input to an action.
 * @ingroup Core_Events
 *
 * Combines a device, key code, and event type. Multiple bindings can
 * be attached to a single InputAction.
 */
struct InputBinding
{
    std::shared_ptr<InputDevice> device; ///< The input device.
    DN_Keycode key;                      ///< Key or button code.
    Input::KeyEvent event;               ///< Event type (pressed, released, held).
    size_t bindingHash;                  ///< Precomputed hash for fast comparison.

    /**
     * @brief Constructs a binding.
     * @param device The input device.
     * @param key The key code.
     * @param event The event type.
     */
    InputBinding(std::shared_ptr<InputDevice> device, DN_Keycode key, Input::KeyEvent event)
        : device(device), key(key), event(event)
    {
        size_t h1 = std::hash<UUID>()(device->uuid);
        size_t h2 = std::hash<DN_Keycode>()(key);
        size_t h3 = std::hash<Input::KeyEvent>()(event);
        bindingHash = h1 ^ (h2 + 0x9e3779b97f4a7c15ULL + (h1 << 6) + (h1 >> 2)) ^
                      (h3 + 0x9e3779b97f4a7c15ULL + (h2 << 6) + (h2 >> 2));
    }

    /** @brief Checks if this binding's condition is currently met. */
    bool Triggered()
    {
        if (device == nullptr)
        {
            return false;
        }

        return device->GetEvent(key, event);
    }
};

/**
 * @struct InputAction
 * @brief Named action with multiple input bindings.
 * @ingroup Core_Events
 *
 * An action represents a game concept (e.g., "jump", "fire") that can be
 * triggered by any of its associated bindings.
 */
struct InputAction
{
    UUID uuid;                               ///< Unique identifier.
    std::string name;                        ///< Action name for lookup.
    std::vector<InputBinding> inputBindings; ///< Associated input bindings.
};

/**
 * @brief Creates a new named input action.
 * @param actionName Unique name for the action.
 */
void CreateInputAction(const std::string &actionName);

/**
 * @brief Adds an input binding to an action.
 * @param actionName The action to modify.
 * @param device The input device.
 * @param key The key code.
 * @param event The event type.
 */
void AddInputActionBinding(const std::string &actionName, std::shared_ptr<InputDevice> device, DN_Keycode key,
                           Input::KeyEvent event);

/** @brief Adds an existing InputBinding to an action. */
void AddInputActionBinding(const std::string &actionName, const InputBinding &newBinding);

/** @brief Removes a specific binding from an action. */
void RemoveInputActionBinding(const std::string &actionName, std::shared_ptr<InputDevice> device, DN_Keycode key,
                              Input::KeyEvent event);

/** @brief Removes a binding from an action by InputBinding. */
void RemoveInputActionBinding(const std::string &actionName, const InputBinding &binding);

/** @brief Removes an action and all its bindings. */
void RemoveInputAction(const std::string &actionName);

/**
 * @brief Checks if an action is currently triggered.
 * @param actionName The action name.
 * @return True if any binding for this action is triggered.
 */
bool IsInputActionTriggered(const std::string &actionName);

/**
 * @brief Registers a callback for when an action is triggered.
 * @param actionName The action name.
 * @param callback Function to call when triggered.
 */
void OnInputActionTriggered(const std::string &actionName, std::function<void(void)> callback);

} // namespace duin
