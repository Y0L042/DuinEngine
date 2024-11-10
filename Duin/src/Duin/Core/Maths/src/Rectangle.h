#pragma once

#include "../MathsCore.h"
#include "Vector2.h"

namespace Duin
{
	struct DUIN_MATHS_API Rectangle
	{
		Vector2 origin;
		float width, height;

		Rectangle()
			: origin(Vector2::ZERO), width(0), height(0)
		{}

		Rectangle(Vector2 origin, float width, float height)
			: origin(origin), width(width), height(height)
		{}

		Rectangle(float x, float y, float width, float height)
			: origin(Vector2{ x, y }), width(width), height(height)
		{}

		bool Contains(Vector2 point) const
		{
			return (point.x >= origin.x && point.x <= origin.x + width
				&& point.y >= origin.y && point.y <= origin.y + height);
		}

		bool Contains(float px, float py) const
		{
			return Contains(Vector2{ px, py });
		}

		bool Intersects(Rectangle& other) const
		{
			return !(origin.x + width < other.origin.x || origin.x > other.origin.x + other.width 
				|| origin.y + height < other.origin.y || origin.y > other.origin.y + other.height);
		}

		Vector2 GetCenter() const
		{
			return origin + (Vector2{ width, height } / 2);
		}

		Vector2 GetCorner() const
		{
			return origin + Vector2{ width, height };
		}

		Rectangle Scale(Vector2 scale)
		{
			width *= scale.x;
			height *= scale.y;
			return *this;
		}
	};
}