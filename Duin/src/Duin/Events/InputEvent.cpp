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
		return ::IsKeyPressed(key);
	}

	bool InputEvent::IsKeyPressedRepeat(int key)
	{
		return ::IsKeyPressedRepeat(key);
	}

	bool InputEvent::IsKeyDown(int key)
	{
		return ::IsKeyDown(key);
	}

	bool InputEvent::IsKeyReleased(int key)
	{
		return ::IsKeyReleased(key);
	}

	bool InputEvent::IsKeyUp(int key)
	{
		return ::IsKeyUp(key);
	}

	bool InputEvent::GetKeyPressed()
	{
		return ::GetKeyPressed();
	}

	bool InputEvent::GetCharPressed()
	{
		return ::GetCharPressed();
	}
	Vector2 InputEvent::GetInputVector(int keyUp, int keyDown, int keyLeft, int keyRight)
	{
		Vector2 vec;
		vec.x = IsKeyDown(keyLeft) * -1 + IsKeyDown(keyRight);
		vec.y = IsKeyDown(keyUp) * -1 + IsKeyDown(keyDown);
		return vec.Normalized();
	}
	Vector2 InputEvent::GetMousePosition()
	{
		return Vector2(::GetMouseX(), ::GetMouseY());
	}
}