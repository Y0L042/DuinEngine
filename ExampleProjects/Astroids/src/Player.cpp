#include "Player.h"

Player::Player(std::shared_ptr<Duin::Blackboard> sharedData)
	:
	registry(sharedData->GetValueCast<std::shared_ptr<Duin::Registry>>("REG")),
	assetManager(sharedData->GetValueCast<std::shared_ptr<Duin::AssetManager>>("ASSETMGR")),
	quadTree(sharedData->GetValueCast<std::shared_ptr<Duin::QuadTree>>("QTREE"))
{
	std::shared_ptr<Duin::Texture> texture = std::make_shared<Duin::Texture>(assetManager.get(), "assets/pixspaceship.png");
	texture->SetTextureWidthKeepRatio(35.0f);

	entity = Duin::Entity::Create(registry.get());
	entity->AddComponent<Component_PlayerInput>();
	entity->AddComponent<Component_Movement>(100.0f, 100.0f);
	entity->AddComponent<Component_Transform>(Duin::Vector2{ 100, 100 });
	entity->AddComponent<Component_Renderable>(texture, true);
}

Player::~Player()
{
}

void Player::PhysicsUpdate(double pDelta)
{
}

void Player::Draw()
{
}
