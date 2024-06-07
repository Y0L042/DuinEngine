#pragma once

#include <Duin.h>

#include "Component_PlayerInput.h"
#include "Component_Movement.h"

struct Handler_PlayerInput
{
	Duin::Registry* registry;

	Handler_PlayerInput() = default;
	Handler_PlayerInput(Duin::Registry* registry)
		: registry(registry)
	{}

	void Update(Duin::InputEvent e)
	{
		auto view = registry->View<Component_PlayerInput, Component_Movement>();
		for (auto [entity, c_pinput, c_movement] : view.each())
		{
			c_movement.inputDir = e.GetInputVector(Duin::KEY_W, Duin::KEY_S, Duin::KEY_A, Duin::KEY_D);
		}
	}
};