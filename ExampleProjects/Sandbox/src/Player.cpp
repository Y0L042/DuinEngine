#include "Player.h"

Player::Player()
{
}

Player::~Player()
{
}

void Player::Initialize()
{
	DN_INFO("Player Initializing");
	//sprite = InstantiateChild<Duin::Sprite2D>("Textures/at_symbol.png")->SetTextureSize(50, 50);
	//sprite->SetLocalPosition2D({ 50, 50 });
}

void Player::PhysicsUpdate(double pDelta)
{
	DN_INFO("Player PhysicsUpdate()");
}
 
