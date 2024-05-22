#include "Player.h"

Player::Player() {}
Player::~Player() {}

void Player::Initialize()
{
	sprite = InstantiateChild<Duin::Sprite2D>("Textures/at_symbol.png");
	sprite->SetTextureSize(50, 50);
}

Duin::Vector2 pos(10, 10);
void Player::PhysicsUpdate(double pDelta)
{
	SetLocalPosition2D(pos);
}

void Player::HandleInput(Duin::InputEvent e)
{
	pos = e.GetMousePosition();
}
