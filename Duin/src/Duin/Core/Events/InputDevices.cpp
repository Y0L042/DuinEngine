#include "dnpch.h"
#include "InputDevices.h"

std::shared_ptr<duin::InputDevice_Keyboard> duin::GetKeyboard_01()
{
    static std::shared_ptr<InputDevice_Keyboard> d = std::make_shared<InputDevice_Keyboard>();
    return d;
}

std::shared_ptr<duin::InputDevice_Mouse> duin::GetMouse_01()
{
    static std::shared_ptr<InputDevice_Mouse> d = std::make_shared<InputDevice_Mouse>();
    return d;
}

std::shared_ptr<duin::InputDevice_Keyboard> duin::GetKeyboard_02()
{
    static std::shared_ptr<InputDevice_Keyboard> d = std::make_shared<InputDevice_Keyboard>();
    return d;
}

std::shared_ptr<duin::InputDevice_Mouse> duin::GetMouse_02()
{
    static std::shared_ptr<InputDevice_Mouse> d = std::make_shared<InputDevice_Mouse>();
    return d;
}
