#pragma once

#include <Duin.h>

#include "Components.h"

#include <vector>
#include <queue>

//	Handler_PlayerInput(Duin::Registry* registry)
struct Handler_PlayerInput
{
	Duin::Registry* registry;

	Handler_PlayerInput() = default;
	Handler_PlayerInput(Duin::Registry* registry)
		: registry(registry)
	{}

	void Update(Duin::InputEvent e);
};


// 	Handler_PlayerMovement(Duin::Registry* registry)
struct Handler_PlayerMovement
{
	Duin::Registry* registry;

	Handler_PlayerMovement() = default;
	Handler_PlayerMovement(Duin::Registry* registry)
		: registry(registry)
	{}

	void Update(double delta);
};

// 	Handler_Renderable(Duin::Registry* registry)
struct Handler_Renderable
{
	Duin::Registry* registry;

	Handler_Renderable() = default;
	Handler_Renderable(Duin::Registry* registry)
		: registry(registry)
	{}

	void Update();
};

//	Handler_RenderableEntity(Duin::Registry* registry)
struct Handler_RenderableEntity
{
	Duin::Registry* registry;

	Handler_RenderableEntity() = default;
	Handler_RenderableEntity(Duin::Registry* registry)
		: registry(registry)
	{}

	void Update();
};


// 	Handler_UpdateTransform(Duin::Registry* registry, int borderWidth, int borderHeight)
struct Handler_UpdateTransform
{
	Duin::Registry* registry;

	int width, height = 0;

	Handler_UpdateTransform() = default;
	Handler_UpdateTransform(Duin::Registry* registry, int borderWidth, int borderHeight)
		: registry(registry), width(borderWidth), height(borderHeight)
	{}

	void Update(double delta);
};

struct Handler_ProcessBoids
{
	Duin::Registry* registry;

	Handler_ProcessBoids() = default;
	Handler_ProcessBoids(Duin::Registry* registry)
		: registry(registry)
	{}

	/*
	* for local_boids (exl this):
	*	cohesion: move towards boids' avg position
	*	alignment: get boids' avg velocity
	*	separation: get avg vec away from boids position
	*/
	void Update(double delta);
};

struct Handler_PlayerBoidMovement
{
	Duin::Registry* registry;

	Handler_PlayerBoidMovement() = default;
	Handler_PlayerBoidMovement(Duin::Registry* registry)
		: registry(registry)
	{}

	void Update(double delta);
};

struct Handler_BoidsFollowLeader
{
	Duin::Registry* registry;

	Handler_BoidsFollowLeader() = default;
	Handler_BoidsFollowLeader(Duin::Registry* registry)
		: registry(registry)
	{}

	void Update(double delta);
};