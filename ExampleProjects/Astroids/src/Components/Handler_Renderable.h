#pragma once

#include <Duin.h>

#include "Component_Renderable.h"
#include "Component_Transform.h"

struct Handler_Renderable
{
	Duin::Registry* registry;

	Handler_Renderable() = default;
	Handler_Renderable(Duin::Registry* registry)
		: registry(registry)
	{}

	void Update()
	{
		auto view = registry->View<Component_Renderable, Component_Transform>();
		for (auto [entity, c_renderable, c_transform] : view.each())
		{
			c_renderable.texture->Draw(c_transform.position);
		}
	}
};