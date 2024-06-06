#pragma once

#include "Duin/Core/Core.h"
#include "Duin/Core/Structures/RenderStructs.h"

#include <RLImGuiComponent.h>

namespace Duin
{
	class DUIN_API DebugDraw
	{
	public:
		static void DrawLine(int x1, int y1, int x2, int y2, Color color);
	};
}