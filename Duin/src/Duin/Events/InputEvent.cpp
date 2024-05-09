#include "dnpch.h"
#include "InputEvent.h"

namespace Duin
{
	InputEvent::InputEvent()
	{
	}

	InputEvent::~InputEvent()
	{
	}

	bool InputEvent::IsKeyPressed(int key)
	{
		return raylib::Keyboard::IsKeyPressed(key);
	}

	bool InputEvent::IsKeyPressedRepeat(int key)
	{
		return raylib::Keyboard::IsKeyPressedRepeat(key);
	}

	bool InputEvent::IsKeyDown(int key)
	{
		return raylib::Keyboard::IsKeyDown(key);
	}

	bool InputEvent::IsKeyReleased(int key)
	{
		return raylib::Keyboard::IsKeyReleased(key);
	}

	bool InputEvent::IsKeyUp(int key)
	{
		return raylib::Keyboard::IsKeyUp(key);
	}

	bool InputEvent::GetKeyPressed()
	{
		return raylib::Keyboard::GetKeyPressed();
	}

	bool InputEvent::GetCharPressed()
	{
		return raylib::Keyboard::GetCharPressed();
	}
}