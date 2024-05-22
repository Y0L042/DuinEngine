#include "Player.h"

Player::Player() {}
Player::~Player() {}

void Player::Initialize()
{
	sprite = InstantiateChild<Duin::Sprite2D>("Textures/at_symbol.png");
	sprite->SetTextureSize(50, 50);
}

float x = 0;
void Player::PhysicsUpdate(double pDelta)
{
	SetLocalPosition2D(x, 10);
	x++;
	DN_INFO("x: {}, y: {}", ::GetMouseX(), ::GetMouseY());
}


 
