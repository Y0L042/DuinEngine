#include "PlayerBoids.h"

PlayerBoids::PlayerBoids(
	std::shared_ptr<Duin::Blackboard> sharedData,
	std::shared_ptr<Duin::Entity> leader
)
	: 
	registry(sharedData->GetValueCast<std::shared_ptr<Duin::Registry>>("REG")),
	assetManager(sharedData->GetValueCast<std::shared_ptr<Duin::AssetManager>>("ASSETMGR")),
	quadTree(sharedData->GetValueCast<std::shared_ptr<Duin::QuadTree>>("QTREE")),
	leader(leader)
{
}


PlayerBoids::~PlayerBoids()
{
}

void PlayerBoids::Ready()
{
	const int BOIDCOUNT = 100;
	for (int i = 0; i < BOIDCOUNT; i++)
	{
		CreateBoid(Duin::Vector2(
			Duin::Random::GetRandomFloat(300, 500), 
			Duin::Random::GetRandomFloat(300, 500))
		);
	}
	DN_INFO("Boids Ready");
}

void PlayerBoids::PhysicsUpdate(double pDelta)
{
}

void PlayerBoids::Draw()
{
}

void PlayerBoids::CreateBoid(Duin::Vector2 position)
{
	static std::shared_ptr<Duin::Texture> texture = std::make_shared<Duin::Texture>(assetManager.get(), "assets/boidship.png");
	texture->SetTextureWidthKeepRatio(22.5f);

	Duin::UUID boidGroupID;

	std::shared_ptr<Duin::Entity> boid = Duin::Entity::Create(registry.get());
	boid->AddComponent<Component_Movement>(10.0f, 10.0f);
	boid->AddComponent<Component_Transform>(position);
	boid->AddComponent<Component_Renderable>(texture, true);
	boid->AddComponent<Component_Boid>();
	boid->AddComponent<Component_BoidGroup>(boidGroupID, leader, 75.0f);
}
