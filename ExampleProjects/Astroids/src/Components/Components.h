#pragma once

#include <Duin.h>

#include <memory>
#include <utility>

struct Component_AstroidInput
{
	bool enableStatusComponent = true;
};

//	Component_Movement(float maxSpeed, float acceleration)
struct Component_Movement
{
	float maxLinearSpeed = 25.0f;
	float maxRotationSpeed = 1.75f;
	float accelerationFactor = 3.5f;
	float brakingFactor = 1.0f;

	std::vector<std::pair<Duin::Vector2, float>> inputVelocityList;

	float currRotation = 0.0f;
	float currSpeed = 0.0f;
	Duin::Vector2 prevVelocity;
	Duin::Vector2 currVelocity;

	Duin::Vector2 inputVel;
	float targetSpeed = 0.0f;
	float targetRotation = 0.0f;

	Component_Movement() = default;
	Component_Movement(float maxSpeed = 25.0f, float acceleration = 3.5f)
		: maxLinearSpeed(maxSpeed), accelerationFactor(acceleration)
	{}
};

//	Component_Transform(Duin::Vector2 position)
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

// 	Component_Renderable(std::shared_ptr<Duin::Texture> texture, Duin::Vector2 size)
struct Component_Renderable
{
	std::shared_ptr<Duin::Texture> texture;
	Duin::Vector2 size;
	Duin::Color tint = Duin::WHITE;
	bool pointInInputDir = false;

	Component_Renderable(std::shared_ptr<Duin::Texture> texture)
		: texture(texture)
	{
	}

	Component_Renderable(std::shared_ptr<Duin::Texture> texture, bool pointInInputDir)
		: texture(texture), pointInInputDir(pointInInputDir)
	{
	}
};

// 	Component_Boid(float cohesionFactor, float alignmentFactor, float separationFactor)
struct Component_Boid
{
	float cohesionFactor = 3.0f;
	float alignmentFactor = 0.5f;
	float separationFactor = 0.25f;

	Component_Boid() {}
	Component_Boid(float cohesionFactor, float alignmentFactor, float separationFactor)
		: cohesionFactor(cohesionFactor), alignmentFactor(alignmentFactor), separationFactor(separationFactor)
	{}
};

//	Component_BoidGroup(Duin::UUID groupID, Duin::Entity* leader)
struct Component_BoidGroup
{
	Duin::UUID groupID;
	std::shared_ptr<Duin::Entity> leader;
	float leaderAttractionFactor = 1.0f;

	Component_BoidGroup() = default;
	Component_BoidGroup(Duin::UUID groupID, std::shared_ptr<Duin::Entity> leader, float leaderAttractionFactor = 1.0f)
		: groupID(groupID), leader(leader), leaderAttractionFactor(leaderAttractionFactor)
	{}
};
