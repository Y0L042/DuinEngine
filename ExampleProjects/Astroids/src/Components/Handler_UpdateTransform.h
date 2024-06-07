#pragma once

#include <Duin.h>

#include "Component_Transform.h"
#include "Component_Movement.h"

struct Handler_UpdateTransform
{
	Duin::Registry* registry;

	int width, height = 0;

	Handler_UpdateTransform() = default;
	Handler_UpdateTransform(Duin::Registry* registry, int borderWidth, int borderHeight)
		: registry(registry), width(borderWidth), height(borderHeight)
	{}

	void Update(double delta)
	{
		Duin::Vector2 testPos;

		auto view = registry->View<Component_Transform, Component_Movement>();
		for (auto [entity, c_transform, c_movement] : view.each())
		{
			c_transform.prevPosition = c_transform.position;

			testPos = c_transform.position;
			testPos += c_movement.currVelocity * delta;

			if ((testPos.x < 0 || testPos.x > width) || (testPos.y < 0 || testPos.y > height))
			{
				c_movement.currVelocity = Duin::Vector2::ZERO;
			}
			else
			{
				c_transform.position = testPos;
			}
		}
	}
};