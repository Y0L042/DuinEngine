#pragma once

#include "InputDevice_Keyboard.h"
#include "InputDevice_Mouse.h"

#include <memory>

#define DN_KEYBOARD_01 duin::GetKeyboard_01()
#define DN_MOUSE_01 duin::GetMouse_01()

#define DN_KEYBOARD_02 duin::GetKeyboard_02()
#define DN_MOUSE_02 duin::GetMouse_02()

namespace duin {
	std::shared_ptr<InputDevice_Keyboard> GetKeyboard_01();
	std::shared_ptr<InputDevice_Mouse> GetMouse_01();

	std::shared_ptr<InputDevice_Keyboard> GetKeyboard_02();
	std::shared_ptr<InputDevice_Mouse> GetMouse_02();
}