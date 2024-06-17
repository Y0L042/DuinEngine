#pragma once

#include <Duin.h>

#include "Components/Components.h"

#include <random>

// PlayerBoids(Duin::Registry* registry, Duin::AssetManager* assetManager, std::shared_ptr<Duin::Entity> leader);
class PlayerBoids : public Duin::Node
{
public:
	std::shared_ptr<Duin::Entity> entity;

	PlayerBoids(Duin::Registry* registry, Duin::AssetManager* assetManager, std::shared_ptr<Duin::Entity> leader);
	~PlayerBoids();

	void Ready() override;
	void PhysicsUpdate(double pDelta) override;
	void Draw() override;

	void CreateBoid(Duin::Vector2 position);

private:
	Duin::Registry* registry = nullptr;
	Duin::AssetManager* assetManager = nullptr;
	std::shared_ptr<Duin::Entity> leader = nullptr;

};