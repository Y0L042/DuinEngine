#include "PlayerBoids.h"

PlayerBoids::PlayerBoids(Duin::Registry* registry, Duin::AssetManager* assetManager)
	: registry(registry), assetManager(assetManager)
{
}


PlayerBoids::~PlayerBoids()
{
}

void PlayerBoids::Ready()
{
	CreateBoid();
	DN_INFO("Boids Ready");
}

void PlayerBoids::PhysicsUpdate(double pDelta)
{
}

void PlayerBoids::Draw()
{
}

void PlayerBoids::CreateBoid()
{
	static std::shared_ptr<Duin::Texture> texture = std::make_shared<Duin::Texture>(assetManager, "assets/boidship.png");
	texture->SetTextureWidthKeepRatio(22.5f);

	std::shared_ptr<Duin::Entity> entity = Duin::Entity::Create(registry);
	entity->AddComponent<Component_Movement>();
	entity->AddComponent<Component_Transform>(Duin::Vector2(25, 25));
	entity->AddComponent<Component_Renderable>(texture);
}
