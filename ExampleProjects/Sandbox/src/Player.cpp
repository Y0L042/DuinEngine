#include "Player.h"

Player::Player()
{
}

Player::~Player()
{
}

void Player::Initialize()
{
	sprite = InstantiateChild<Duin::Sprite2D>("Textures/at_symbol.png")->SetTextureSize(50, 50);
	//sprite->SetGlobalPosition2D({ 50, 50 });
}
