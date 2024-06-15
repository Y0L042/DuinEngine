#pragma once

#include <Duin.h>

#include "Components/PlayerComponent.h"
#include "Components/PlayerInputComponent.h"
#include "Components/PlayerCMPManager.h"
#include "Components/MovementComponent.h"
#include "Components/MovementCMPManager.h"
#include "Components/TransformComponent.h"
#include "Components/TransformCMPManager.h"
#include "Components/RenderableComponent.h"
#include "Components/RenderableCMPManager.h"
#include "Components/NewTransfromCMPManager.h"

#include <random>

#include <CDT.h>

class Sandbox : public Duin::Application
{
public:
	Sandbox() {}
	~Sandbox() {}

	void Initialize() override;
	void Ready() override;
	void HandleInputs(Duin::InputEvent e) override;
	void Update(double rDelta) override;
	void PhysicsUpdate(double pDelta) override;
	void Draw() override;

	void SpawnEntityBatches();
	void SpawnEntity(Duin::Vector2 position, Duin::Vector2 velocity);

	int min = 20;
	int max = 700;

	std::random_device rd;  // Obtain a random number from hardware
	std::mt19937 gen; // Seed the generator
	std::uniform_int_distribution<> distr; // Define the range


	std::shared_ptr<Duin::Registry> registry;
	std::shared_ptr<Duin::AssetManager> assetManager;
	NewTransformCMPManager* newTFCMPManager;
	int entityCount = 0;
	std::shared_ptr<Duin::Texture> texture;
	Duin::QuadTree qTree;
	Duin::Profiler profiler;
	int numEntities = 2500;
};