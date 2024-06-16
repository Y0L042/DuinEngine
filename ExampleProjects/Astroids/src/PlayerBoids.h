#pragma once

#include <Duin.h>

#include "Components/Components.h"

//	PlayerBoids(Duin::Registry* registry, Duin::AssetManager* assetManager);
class PlayerBoids : public Duin::Node
{
public:
	std::shared_ptr<Duin::Entity> entity;

	PlayerBoids(Duin::Registry* registry, Duin::AssetManager* assetManager);
	~PlayerBoids();

	void Ready() override;
	void PhysicsUpdate(double pDelta) override;
	void Draw() override;

	void CreateBoid();

private:
	Duin::Registry* registry = nullptr;
	Duin::AssetManager* assetManager = nullptr;

};