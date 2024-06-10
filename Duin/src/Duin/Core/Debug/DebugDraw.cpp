#include "dnpch.h"

#include "DebugDraw.h"

namespace Duin
{
	void DebugDraw::DrawLine(int x1, int y1, int x2, int y2, Color color)
	{
		::Color rlColor{ color.r, color.g, color.b, color.a };
		::DrawLine(x1, y1, x2, y2, rlColor);
	}

	void DebugDraw::DrawLine(Vector2 v1, Vector2 v2, Color color)
	{
		::Color rlColor{ color.r, color.g, color.b, color.a };
		::DrawLine((int)v1.x, (int)v1.y, (int)v2.x, (int)v2.y, rlColor);
	}

	void DebugDraw::DrawTriangleLines(Vector2 v1, Vector2 v2, Vector2 v3, Color color)
	{
		::Color rlColor{ color.r, color.g, color.b, color.a };
		::Vector2 rlVec1{ v1.x, v1.y };
		::Vector2 rlVec2{ v2.x, v2.y };
		::Vector2 rlVec3{ v3.x, v3.y };
		::DrawTriangleLines(rlVec1, rlVec2, rlVec3, rlColor);
	}

	void DebugDraw::DrawCircle(Vector2 center, float radius, Color color)
	{
		::Color rlColor{ color.r, color.g, color.b, color.a };
		::Vector2 rlCenter{ center.x, center.y };
		::DrawCircleV(rlCenter, radius, rlColor);
	}

	void DebugDraw::DrawCircleLines(Vector2 center, float radius, Color color)
	{
		::Color rlColor{ color.r, color.g, color.b, color.a };
		::Vector2 rlCenter{ center.x, center.y };
		::DrawCircleLinesV(rlCenter, radius, rlColor);
	}
}