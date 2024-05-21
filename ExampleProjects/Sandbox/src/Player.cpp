#include "Player.h"

Player::Player()
{
}

Player::~Player()
{
}

void Player::Initialize()
{
	sprite = InstantiateChild<Duin::Sprite2D>("Textures/at_symbol.png");
	sprite->SetTextureSize(50, 50);
	sprite->SetLocalPosition2D({ 50, 50 });
}

void Player::PhysicsUpdate(double pDelta)
{
}

void Player::Draw()
{
}
 
