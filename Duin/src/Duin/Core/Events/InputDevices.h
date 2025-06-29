#pragma once

#include "InputDevice_Keyboard.h"
#include "InputDevice_Mouse.h"

#include <memory>

namespace duin {

#define DN_KEYBOARD_1 GetKeyboard_01()
#define DN_MOUSE_1 GetMouse_01()

#define DN_KEYBOARD_2 GetKeyboard_02()
#define DN_MOUSE_2 GetMouse_02()

	std::shared_ptr<InputDevice_Keyboard> GetKeyboard_01();
	std::shared_ptr<InputDevice_Mouse> GetMouse_01();

	std::shared_ptr<InputDevice_Keyboard> GetKeyboard_02();
	std::shared_ptr<InputDevice_Mouse> GetMouse_02();
}