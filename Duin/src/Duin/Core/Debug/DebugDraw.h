#pragma once

#include "Duin/Core/Core.h"
#include "Duin/Core/Structures/RenderStructs.h"
#include "Duin/Core/Maths/DuinMaths.h"

#include <RLImGuiComponent.h>

#include <functional>

namespace Duin
{
	class DUIN_API DebugDraw
	{
	public:
		static void DrawLine(int x1, int y1, int x2, int y2, Color color);
		static void DrawLine(Vector2 v1, Vector2 v2, Color color);

		static void DrawTriangleLines(Vector2 v1, Vector2 v2, Vector2 v3, Color color);

		static void DrawCircle(Vector2 center, float radius, Color color);
		static void DrawCircleLines(Vector2 center, float radius, Color color);

		static void Draw();

	private:
		std::vector<std::function<void()>> drawQueue;

		static DebugDraw& GetInstance()
		{
			static DebugDraw* instance = new DebugDraw();
			return *instance;
		}

		DebugDraw() {}
		~DebugDraw() {}
	};
}