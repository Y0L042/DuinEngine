#pragma once

#pragma once

#include <Duin.h>

#include "Component_Movement.h"
#include "Component_Renderable.h"
#include "Component_Transform.h"

struct Handler_RenderableEntity
{
	Duin::Registry* registry;

	Handler_RenderableEntity() = default;
	Handler_RenderableEntity(Duin::Registry* registry)
		: registry(registry)
	{}

	void Update()
	{
		auto view = registry->View<Component_Movement, Component_Renderable, Component_Transform>();
		for (auto [entity, c_movement, c_renderable, c_transform] : view.each())
		{
			float rot = 0.0f;
			if (c_renderable.pointInInputDir)
			{
				rot = c_movement.currRotation;
			}
			c_renderable.texture->Draw(c_transform.position, rot);
		}
	}
};