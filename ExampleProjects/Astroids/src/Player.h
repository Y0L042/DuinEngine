#pragma once

#include <Duin.h>

#include "Components/Components.h"

class Player : public Duin::Node
{
public:
	std::shared_ptr<Duin::Entity> entity;

	Player(Duin::Registry* registry);
	~Player();

	void PhysicsUpdate(double pDelta) override;
};