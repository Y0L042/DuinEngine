#pragma once

#include <RLImGuiComponent.h>
#include "Duin/Core/Maths/DuinMaths.h"

namespace RLW
{
	::Vector2 D2R_Vector2(Duin::Vector2 vec2)
	{
		return ::Vector2{ vec2.x, vec2.y };
	}

	Duin::Vector2 R2D_Vector2(::Vector2 vec2)
	{
		return Duin::Vector2{ vec2.x, vec2.y };
	}

	::Color D2R_Color(Duin::Color c)
	{
		return ::Color{ c.r, c.b, c.g, c.a };
	}

	Duin::Color R2D_Color(::Color c)
	{
		return Duin::Color{ c.r, c.b, c.g, c.a };
	}
}