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
		int GetKeyPressed();
		int GetCharPressed();
		Vector2 GetInputVector(int keyUp, int keyDown, int keyLeft, int keyRight);

		bool IsMouseButtonPressed(int button);                  // Check if a mouse button has been pressed once
		bool IsMouseButtonDown(int button);                     // Check if a mouse button is being pressed
		bool IsMouseButtonReleased(int button);                 // Check if a mouse button has been released once
		bool IsMouseButtonUp(int button);                       // Check if a mouse button is NOT being pressed
		int GetMouseX(void);                                    // Get mouse position X
		int GetMouseY(void);                                    // Get mouse position Y
		Vector2 GetMousePosition(void);                         // Get mouse position XY
		Vector2 GetMouseDelta(void);                            // Get mouse delta between frames
		void SetMousePosition(int x, int y);                    // Set mouse position XY
		void SetMouseOffset(int offsetX, int offsetY);          // Set mouse offset
		void SetMouseScale(float scaleX, float scaleY);         // Set mouse scaling
		float GetMouseWheelMove(void);                          // Get mouse wheel movement for X or Y, whichever is larger
		Vector2 GetMouseWheelMoveV(void);                       // Get mouse wheel movement for both X and Y
		void SetMouseCursor(int cursor);

	private:
	};
}