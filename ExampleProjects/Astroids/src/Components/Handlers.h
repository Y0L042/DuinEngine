#pragma once

#include <Duin.h>

#include "Components.h"

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
			float speed = (c_movement.maxSpeed * e.IsKeyDown(Duin::KEY_W)) - (c_movement.braking * e.IsKeyDown(Duin::KEY_S));
			c_movement.currRotation += c_movement.maxRotationSpeed * (e.IsKeyDown(Duin::KEY_A) * -1) * e.IsKeyDown(Duin::KEY_D);
			c_movement.inputDir = Duin::Vector2::AngleToVector2(Duin::Maths::DegreesToRadians(c_movement.currRotation));
			c_movement.inputDir = (c_movement.inputDir * speed).Normalized();
		}
	}
};

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
			if (c_movement.inputDir.Magnitude() > 0)
			{
				c_movement.currRotation = Duin::Maths::RadiansToDegrees(c_movement.currVelocity.ToAngle()) + 90.0f;
			}
		}
	}
};

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
			c_renderable.texture->Draw(c_transform.position, 0);
		}
	}
};

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