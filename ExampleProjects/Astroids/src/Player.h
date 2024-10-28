#pragma once

#include <Duin.h>

#include "Components/Components.h"

class Player : public Duin::Node
{
public:
	std::shared_ptr<Duin::Entity> entity;

	Player(
		std::shared_ptr<Duin::Blackboard> sharedData
	);
	~Player();

	void PhysicsUpdate(double pDelta) override;
	void Draw() override;

private:
	std::shared_ptr<Duin::Registry> registry = nullptr;
	std::shared_ptr<Duin::AssetManager> assetManager = nullptr;
	std::shared_ptr<Duin::QuadTree> quadTree = nullptr;
};