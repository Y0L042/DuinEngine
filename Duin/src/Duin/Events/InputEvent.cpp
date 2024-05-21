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
}