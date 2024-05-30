#include <Duin.h>
#include <Duin/EntryPoint.h>

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

int min = 20;
int max = 700;
std::random_device rd;  // Obtain a random number from hardware
std::mt19937 gen(rd()); // Seed the generator
std::uniform_int_distribution<> distr(min, max); // Define the range

std::shared_ptr<Duin::TextureResource> texture;

std::shared_ptr<Duin::Registry> registry;
NewTransformCMPManager* newTFCMPManager;
int entityCount = 0;

Duin::QuadTree qTree(Duin::Rectangle(0.0f, 0.0f, 1280.0f, 720.0f), 5, 1000, 0);

class Sandbox : public Duin::Application
{
public:
	Sandbox() {}
	~Sandbox() {}
	
	void Initialize() override;
	void Ready() override;
	void HandleInputs(Duin::InputEvent e) override;
	void Process(double rDelta) override;
	void PhysicsProcess(double pDelta) override;
	void Draw() override;

	void SpawnEntityBatches();
};
Duin::Application* Duin::CreateApplication() { return new Sandbox(); }



void Sandbox::Initialize()
{
	registry = GetSceneManager().GetRegistry();
	newTFCMPManager = &GetSceneManager().CreateComponentManager<NewTransformCMPManager>();

	texture = std::make_shared<Duin::TextureResource>("Textures/at_symbol.png");
}

void Sandbox::Ready()
{

}

void Sandbox::HandleInputs(Duin::InputEvent e)
{
	PlayerCMPManager::Update(registry.get(), e);
}

void Sandbox::Process(double rDelta)
{
}

int numEntities = 10000;

void Sandbox::PhysicsProcess(double pDelta)
{
	if (numEntities > entityCount)
	{
		SpawnEntityBatches();
	}

	MovementCMPManager::Update(registry.get(), pDelta);
	TransformCMPManager::Update(registry.get(), pDelta);
	//newTFCMPManager->Update(pDelta);
	
}

void Sandbox::Draw()
{
	RenderableCMPManager::Update(registry.get());
}

void Sandbox::SpawnEntityBatches()
{
	int entityBatch = 100;
	if (entityCount < numEntities)
	{
		for (int i = 0; i < (entityBatch < numEntities - entityCount ? entityBatch : numEntities - entityCount); i++)
		{
			std::shared_ptr<Duin::Entity> entity = GetSceneManager().CreateEntity();
			Duin::Vector2 position{ (float)distr(gen), (float)distr(gen) };

			entity->AddComponent<PlayerCMP>();
			entity->AddComponent<PlayerInputCMP>();
			entity->AddComponent<MovementCMP>(Duin::Vector2{ (float)distr(gen), (float)distr(gen) }.Normalized());
			entity->AddComponent<TransformCMP>(position);
			entity->AddComponent<RenderableCMP>(texture.get(), Duin::Vector2{ 15, 15 });
			auto qtComp = entity->AddComponent<Duin::QuadTreeComponent>(entity->GetUUID(), position);

			qTree.Insert(qtComp);

			entityCount++;
		}
	}
}