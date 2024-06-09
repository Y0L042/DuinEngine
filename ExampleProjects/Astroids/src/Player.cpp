#include "Player.h"

Player::Player(Duin::Registry* registry)
{
	std::shared_ptr<Duin::TextureResource> texture = std::make_shared<Duin::TextureResource>("assets/spaceship.png");
	texture->SetCentered(true);
	entity = Duin::Entity::Create(registry);
	entity->AddComponent<Component_PlayerInput>();
	entity->AddComponent<Component_Movement>();
	entity->AddComponent<Component_Transform>(Duin::Vector2{ 100, 100 });
	entity->AddComponent<Component_Renderable>(texture, Duin::Vector2{ 25, 25 }, true);
}

Player::~Player()
{
}
