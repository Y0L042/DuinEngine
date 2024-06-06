#include "dnpch.h"

#include "DebugDraw.h"

namespace Duin
{
	void DebugDraw::DrawLine(int x1, int y1, int x2, int y2, Color color)
	{
		::Color rlColor = { color.r, color.g, color.b, color.a };
		::DrawLine(x1, y1, x2, y2, rlColor);
	}
}