#include "dnpch.h"
#include "InputActions.h"
#include "Duin/Core/Debug/DNLog.h"

#include <unordered_map>

static std::unordered_map<std::string, duin::InputAction> inputActionMap;

void duin::CreateInputAction(const std::string& actionName)
{
    // Creates empty Action
    if (!inputActionMap.try_emplace(actionName, InputAction()).second) {
        DN_CORE_WARN("Unable to create InputAction {}!", actionName);
    } else {
        inputActionMap[actionName].name = actionName;
    }
}

void duin::AddInputActionBinding(const std::string& actionName, std::shared_ptr<InputDevice> device, DN_Keycode key, Input::KeyEvent event)
{
    AddInputActionBinding(actionName, InputBinding(device, key, event));
}

void duin::AddInputActionBinding(const std::string& actionName, const InputBinding& newBinding)
{
    if (inputActionMap.find(actionName) == inputActionMap.end()) {
        // Create new Action
        DN_CORE_WARN("InputAction {} not found, creating new Action!", actionName);
        if (!inputActionMap.try_emplace(actionName, InputAction()).second) {
            DN_CORE_WARN("Unable to create InputAction {}!", actionName);
            return;
        }
    }

    // Add binding
    auto& action = inputActionMap[actionName];
    for (auto& binding : action.inputBindings) {
        if (newBinding.bindingHash == binding.bindingHash) {
            DN_CORE_WARN("Binding for Action already exist!", actionName);
            return;
        }
    }
    action.inputBindings.push_back(newBinding);
}

void duin::RemoveInputActionBinding(const std::string& actionName, std::shared_ptr<InputDevice> device, DN_Keycode key, Input::KeyEvent event)
{
    RemoveInputActionBinding(actionName, InputBinding(device, key, event));
}

void duin::RemoveInputActionBinding(const std::string& actionName, const InputBinding& binding)
{
    if (inputActionMap.find(actionName) == inputActionMap.end()) {
        DN_CORE_WARN("InputAction {} not found, unable to remove!", actionName);
        return;
    }
    auto& action = inputActionMap[actionName];
    auto it = action.inputBindings.begin();
    while (it != action.inputBindings.end()) {
        if (it->bindingHash == binding.bindingHash) {
            std::swap(*it, action.inputBindings.back());
            action.inputBindings.pop_back();
            break;
        }
    }
}

void duin::RemoveInputAction(const std::string& actionName)
{
    if (inputActionMap.find(actionName) == inputActionMap.end()) {
        DN_CORE_WARN("InputAction {} not found, unable to remove!", actionName);
        return;
    }
    inputActionMap.erase(actionName);
}

bool duin::IsInputActionTriggered(const std::string& actionName)
{
    if (inputActionMap.find(actionName) == inputActionMap.end()) {
        DN_CORE_WARN("InputAction {} not found!", actionName);
        return false;
    }

    auto& action = inputActionMap[actionName];
    for (auto& binding : action.inputBindings) {
        if (binding.Triggered()) {
            return true;
        }
    }

	return false;
}
