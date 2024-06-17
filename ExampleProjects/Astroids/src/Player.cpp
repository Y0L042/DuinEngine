#include "Player.h"

Player::Player(Duin::Registry* registry, Duin::AssetManager* assetManager)
	: registry(registry), assetManager(assetManager)
{
	std::shared_ptr<Duin::Texture> texture = std::make_shared<Duin::Texture>(assetManager, "assets/pixspaceship.png");
	texture->SetTextureWidthKeepRatio(35.0f);
	//texture->SetCentered(true);
	//texture->SetTintColor(Duin::BLUE);

	entity = Duin::Entity::Create(registry);
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
