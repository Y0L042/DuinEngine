#pragma once

#include <Duin.h>

struct TransformCMP
{
	Duin::Vector2 position;
	Duin::Vector2 direction;

	TransformCMP()
		: position(Duin::Vector2::ZERO), direction(Duin::Vector2::ZERO)
	{}

	TransformCMP(Duin::Vector2 i_position)
		: position(i_position), direction(Duin::Vector2::ZERO)
	{}
};