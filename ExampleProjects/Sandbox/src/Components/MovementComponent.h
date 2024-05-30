#pragma once

#include <Duin.h>

struct MovementCMP
{
	Duin::Vector2 velocity;
	Duin::Vector2 prevVelocity;

	MovementCMP() {};
	MovementCMP(Duin::Vector2 randVel)
		: velocity(randVel), prevVelocity(randVel)
	{}

};