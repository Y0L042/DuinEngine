#pragma once

#include <Duin.h>

#include "Components/Components.h"

class Player : public Duin::Node
{
public:
	std::shared_ptr<Duin::Entity> entity;

	Player(Duin::Registry* registry, Duin::AssetManager* assetManager);
	~Player();

	void PhysicsUpdate(double pDelta) override;
	void Draw() override;

private:
	Duin::Registry* registry = nullptr;
	Duin::AssetManager* assetManager = nullptr;

};