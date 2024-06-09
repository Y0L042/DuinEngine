#pragma once

#include <Duin.h>

#include "Components/Components.h"

class Player : public Duin::Node
{
public:
	Player(Duin::Registry* registry);
	~Player();

	std::shared_ptr<Duin::Entity> entity;
};