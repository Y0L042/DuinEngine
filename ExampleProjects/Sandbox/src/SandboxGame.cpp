#include "SandboxGame.h"

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


int min = 20;
int max = 700;
std::random_device rd;  // Obtain a random number from hardware
std::mt19937 gen(rd()); // Seed the generator
std::uniform_int_distribution<> distr(min, max); // Define the range


std::shared_ptr<Duin::Registry> registry;
NewTransformCMPManager* newTFCMPManager;
int entityCount = 0;
std::shared_ptr<Duin::TextureResource> texture;
Duin::QuadTree qTree(Duin::Rectangle(0.0f, 0.0f, 1280.0f, 720.0f), 2, 1000, 0);
Duin::Profiler profiler;



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
	if (e.IsMouseButtonPressed(Duin::MOUSE_BUTTON_LEFT))
	{
		//SpawnEntity(e.GetMousePosition(), Duin::Vector2::ZERO);
		DN_INFO("Mouse clicked");
	}
	PlayerCMPManager::Update(registry.get(), e);
}

void Sandbox::Process(double rDelta)
{
}

int numEntities = 2500;

void Sandbox::PhysicsProcess(double pDelta)
{
	Duin::Timer timer;

	if (numEntities > entityCount)
	{
		SpawnEntityBatches();
	}

	double spawnTime = timer.Lap();
	MovementCMPManager::Update(registry.get(), pDelta);
	double movTime = timer.Lap();
	TransformCMPManager::Update(registry.get(), pDelta);

	double tfTime = timer.Lap();

	qTree.RebuildTree();

	double rebuildTreeTime = timer.Lap();


	std::string spawnProc = fmt::format("Spawn: {} ms", spawnTime / 1000.0f);
	profiler.DrawText(spawnProc);

	std::string mvProc = fmt::format("Mov: {} ms", movTime / 1000.0f);
	profiler.DrawText(mvProc);

	std::string tfProc = fmt::format("TF: {} ms", tfTime / 1000.0f);
	profiler.DrawText(tfProc);

	std::string strrebuildTreeTime = fmt::format("RebQT: {} ms", rebuildTreeTime / 1000.0f);
	profiler.DrawText(strrebuildTreeTime);

	std::string physProc = fmt::format("PhysicsProcess: {} ms", timer.Stop() / 1000.0f);
	profiler.DrawText(physProc);

	std::string formattedString = fmt::format("Entity Count: {}", entityCount);
	profiler.DrawText(formattedString);
}

void Sandbox::Draw()
{
	RenderableCMPManager::Update(registry.get());
	qTree.Draw();
	profiler.Draw();
}

void Sandbox::SpawnEntityBatches()
{
	int entityBatch = 1;
	if (entityCount < numEntities)
	{
		for (int i = 0; i < (entityBatch < numEntities - entityCount ? entityBatch : numEntities - entityCount); i++)
		{
			Duin::Vector2 position{ (float)distr(gen), (float)distr(gen) };
			Duin::Vector2 velocity{ (float)distr(gen), (float)distr(gen) };
			SpawnEntity(position, velocity);
		}
	}
}

void Sandbox::SpawnEntity(Duin::Vector2 position, Duin::Vector2 velocity)
{
	std::shared_ptr<Duin::Entity> entity = GetSceneManager().CreateEntity();
	
	entity->AddComponent<PlayerCMP>();
	entity->AddComponent<PlayerInputCMP>();
	entity->AddComponent<MovementCMP>(velocity);
	entity->AddComponent<TransformCMP>(position);
	entity->AddComponent<RenderableCMP>(texture.get(), Duin::Vector2{ 15, 15 });
	entity->AddComponent<Duin::QuadTreeComponent>(&qTree, entity->GetUUID(), position);

	entityCount++;
}

