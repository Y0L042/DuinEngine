#include "Keybinds.h"
#include <Duin/Core/Events/EventsModule.h>
#include <Duin/Core/Utils/UtilsModule.h>
#include <Duin/Core/Debug/DNLog.h>


const char KEYBIND_HEAD[] = "KEYBINDS";
const char INPUT_ACTIONS_LIST[] = "INPUT_ACTIONS_LIST";
// List of input actions
const char INPUT_ACTION_NAME[] = "INPUT_ACTION_NAME";
const char INPUT_ACTION_BINDINGS_LIST[] = "INPUT_ACTION_BINDINGS_LIST";

// List of bindings for each action
const char INPUT_ACTION_BINDING_UUID[] = "INPUT_ACTION_BINDING_UUID";
const char INPUT_DEVICE_UUID[] = "INPUT_DEVICE_UUID";
const char INPUT_KEY_CODE[] = "INPUT_KEY_CODE";
const char INPUT_KEY_EVENT[] = "INPUT_KEY_EVENT";

void DeserialiseKeybinds(duin::DataValue keybinds)
{
    if (keybinds.HasMember(INPUT_ACTIONS_LIST)) {
        duin::DataValue inputActions = keybinds[INPUT_ACTIONS_LIST];
        for (auto action : inputActions) {
            std::string actionName = action[INPUT_ACTION_NAME].GetString();

            duin::DataValue bindings = action[INPUT_ACTION_BINDINGS_LIST];
            for (auto binding : bindings) {
                // TODO
                // duin::UUID bindingUUID = binding[INPUT_ACTION_BINDING_UUID];
                size_t keyCode = static_cast<size_t>(binding[INPUT_KEY_CODE].GetInt());
                size_t event = static_cast<size_t>(binding[INPUT_KEY_EVENT].GetInt());


                // duin::AddInputActionBinding(actionName, )
            }
        }
    }
}

duin::DataValue SerialiseKeybinds()
{

    return duin::DataValue();
}
