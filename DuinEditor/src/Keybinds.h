#pragma once

#include <Duin/IO/IOModule.h>
#include <string>

extern const char KEYBIND_HEAD[];
    extern const char INPUT_ACTIONS_LIST[];
        // List of input actions
        extern const char INPUT_ACTION_NAME[];
        extern const char INPUT_ACTION_BINDINGS_LIST[];
            // List of bindings for each action
            extern const char INPUT_ACTION_BINDING_UUID[];
                extern const char INPUT_DEVICE_UUID[];
                extern const char INPUT_KEY_CODE[];
                extern const char INPUT_KEY_EVENT[];

/**
 * Make sure that data starts at KEYBIND_HEAD
 */
void DeserialiseKeybinds(duin::DataValue keybinds);
duin::DataValue SerialiseKeybinds();
