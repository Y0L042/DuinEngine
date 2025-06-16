#pragma once

#include "Input.h"
#include "Duin/Core/Utils/UUID.h"

#include <vector>
#include <string>
#include <memory>

namespace duin {
    struct InputDevice {
        UUID uuid;

        virtual bool GetEvent(DN_Keycode key, Input::KeyEvent event)
        {
            return false;
        }
    };

    struct InputBinding {
        std::shared_ptr<InputDevice> device; // struct
        DN_Keycode key; // uint32_t 
        Input::KeyEvent event; // enum

        size_t bindingHash;

        InputBinding(std::shared_ptr<InputDevice> device, DN_Keycode key, Input::KeyEvent event)
            : device(device), key(key), event(event)
        {
            size_t h1 = std::hash<UUID>()(device->uuid);
            size_t h2 = std::hash<DN_Keycode>()(key);
            size_t h3 = std::hash<Input::KeyEvent>()(event);
            bindingHash = h1 ^ (h2 + 0x9e3779b97f4a7c15ULL + (h1<<6) + (h1>>2))
                ^ (h3 + 0x9e3779b97f4a7c15ULL + (h2<<6) + (h2>>2));
        }

        bool Triggered()
        {
            if (device == nullptr) {
                return false;
            }

            return device->GetEvent(key, event);
        }
    };

    struct InputAction {
        UUID uuid;
        std::string name;
        std::vector <InputBinding> inputBindings;
    };

    void CreateInputAction(const std::string& actionName);
    void AddInputActionBinding(const std::string& actionName, 
                               std::shared_ptr<InputDevice> device,
                               DN_Keycode key, 
                               Input::KeyEvent event);
    void AddInputActionBinding(const std::string& actionName, 
                               const InputBinding& newBinding);
    void RemoveInputActionBinding(const std::string& actionName,
                             std::shared_ptr<InputDevice> device,
                             DN_Keycode key,
                             Input::KeyEvent event);
    void RemoveInputActionBinding(const std::string& actionName,
                                  const InputBinding& binding);
    void RemoveInputAction(const std::string& actionName);

    bool IsInputActionTriggered(const std::string& actionName);
}
