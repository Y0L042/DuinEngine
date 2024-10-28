#pragma once

#include "Duin/Core/Core.h"
#include "Duin/Core/Scene/Camera.h"

#include <RLImGuiComponent.h>

namespace Duin
{
	class DUIN_API Camera2D : public Camera
	{
	public:
		Camera2D();
		~Camera2D();

		::Camera2D GetRaylibCamera();

	private:
		Vector2 offset = Vector2::ZERO;
		Vector2 target = Vector2::RIGHT;
		float rotation_deg = 0.0f;
		float zoom = 1.0f;
	};
}