#pragma once

#include "Duin/Core/Core.h"
#include "Duin/Events/InputMap.h"

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

	private:
	};
}