#pragma once

#include <Duin.h>

#include "Components/AllComponents.h"

class Player
{
public:
	Player(Duin::Registry* registry);
	~Player();

	std::shared_ptr<Duin::Entity> entity;
};