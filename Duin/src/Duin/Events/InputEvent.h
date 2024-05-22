#pragma once

#include "Duin/Core/Core.h"
#include "Duin/Events/InputMap.h"
#include "Duin/Core/Maths/DuinMaths.h"

#include <RaylibComponent.h>

namespace Duin
{
	class DUIN_API InputEvent
	{
	public:
		InputEvent();
		~InputEvent();

		bool IsKeyPressed(int key);
		bool IsKeyPressedRepeat(int key);
		bool IsKeyDown(int key);
		bool IsKeyReleased(int key);
		bool IsKeyUp(int key);
		bool GetKeyPressed();
		bool GetCharPressed();
		Vector2 GetInputVector(int keyUp, int keyDown, int keyLeft, int keyRight);

		Vector2 GetMousePosition();

	private:
	};
}