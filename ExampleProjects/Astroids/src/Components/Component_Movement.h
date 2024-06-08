#pragma once

#include <Duin.h>

struct Component_Movement
{
	float maxSpeed = 1.0f;
	float acceleration = 1.0f;

	float currRotation = 0.0f;
	float currSpeed = 0.0f;
	Duin::Vector2 inputDir;
	Duin::Vector2 prevVelocity;
	Duin::Vector2 currVelocity;

	Component_Movement() = default;
	Component_Movement(float maxSpeed, float acceleration)
		: maxSpeed(maxSpeed), acceleration(acceleration)
	{}
};