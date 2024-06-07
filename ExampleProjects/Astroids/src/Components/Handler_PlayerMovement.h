#pragma once

#include <Duin.h>

#include "Component_PlayerInput.h"
#include "Component_Movement.h"

struct Handler_PlayerMovement
{
	Duin::Registry* registry;

	Handler_PlayerMovement() = default;
	Handler_PlayerMovement(Duin::Registry* registry)
		: registry(registry)
	{}

	void Update()
	{
		auto view = registry->View<Component_PlayerInput, Component_Movement>();
		for (auto [entity, c_pinput, c_movement] : view.each())
		{
			c_movement.prevVelocity = c_movement.currVelocity;
			c_movement.currVelocity += c_movement.inputDir * c_movement.acceleration;
			c_movement.currVelocity.LimitLength(0.0f, c_movement.maxSpeed);
		}
	}
};