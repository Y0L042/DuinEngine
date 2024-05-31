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

	int InputEvent::GetKeyPressed()
	{
		return ::GetKeyPressed();
	}

	int InputEvent::GetCharPressed()
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
	bool InputEvent::IsMouseButtonPressed(int button)
	{
		return ::IsMouseButtonPressed(button);
	}
	bool InputEvent::IsMouseButtonDown(int button)
	{
		return ::IsMouseButtonDown(button);
	}
	bool InputEvent::IsMouseButtonReleased(int button)
	{
		return ::IsMouseButtonReleased(button);
	}
	bool InputEvent::IsMouseButtonUp(int button)
	{
		return IsMouseButtonUp(button);
	}
	int InputEvent::GetMouseX(void)
	{
		return ::GetMouseX();
	}
	int InputEvent::GetMouseY(void)
	{
		return ::GetMouseY();
	}
	Vector2 InputEvent::GetMousePosition()
	{
		return Vector2(::GetMouseX(), ::GetMouseY());
	}
	Vector2 InputEvent::GetMouseDelta(void)
	{
		return Vector2(::GetMouseDelta().x, ::GetMouseDelta().y);
	}
	void InputEvent::SetMousePosition(int x, int y)
	{
		::SetMousePosition(x, y);
	}
	void InputEvent::SetMouseOffset(int offsetX, int offsetY)
	{
		::SetMouseOffset(offsetX, offsetY);
	}
	void InputEvent::SetMouseScale(float scaleX, float scaleY)
	{
		::SetMouseScale(scaleX, scaleY);
	}
	float InputEvent::GetMouseWheelMove(void)
	{
		return ::GetMouseWheelMove();
	}
	Vector2 InputEvent::GetMouseWheelMoveV(void)
	{
		return Vector2(::GetMouseWheelMoveV().x, ::GetMouseWheelMoveV().y);
	}
	void InputEvent::SetMouseCursor(int cursor)
	{
		::SetMouseCursor(cursor);
	}
}