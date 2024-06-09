#pragma once

#include <Duin.h>

#include <memory>

struct Component_AstroidInput
{
	bool enableStatusComponent = true;
};

struct Component_Movement
{
	float maxSpeed = 1.0f;
	float acceleration = 1.0f;
	float brakingSpeed = 0.7f;
	float maxRotationSpeed = 1.0f;

	float currRotation = 0.0f;
	float currSpeed = 0.0f;
	Duin::Vector2 prevVelocity;
	Duin::Vector2 currVelocity;

	Duin::Vector2 inputVel;
	float targetSpeed = 0.0f;
	float targetRotation = 0.0f;

	Component_Movement() = default;
	Component_Movement(float maxSpeed, float acceleration)
		: maxSpeed(maxSpeed), acceleration(acceleration)
	{}
};

struct Component_Transform
{
	Duin::Vector2 prevPosition;
	Duin::Vector2 position;

	Component_Transform() {};
	Component_Transform(Duin::Vector2 position)
		: position(position)
	{}
};

struct Component_PlayerInput
{
	bool enableStatusComponent = true;
};

struct Component_Renderable
{
	std::shared_ptr<Duin::TextureResource> texture;
	Duin::Vector2 size;
	bool pointInInputDir = false;

	Component_Renderable(std::shared_ptr<Duin::TextureResource> texture, Duin::Vector2 size)
		: texture(texture), size(size)
	{
		texture->SetTextureSize(size);
	}

	Component_Renderable(std::shared_ptr<Duin::TextureResource> texture, Duin::Vector2 size, bool pointInInputDir)
		: texture(texture), size(size), pointInInputDir(pointInInputDir)
	{
		texture->SetTextureSize(size);
	}
};