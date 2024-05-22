#include "Player.h"

Player::Player() {}
Player::~Player() {}

Duin::Vector2 input;

void Player::Initialize()
{
	sprite = InstantiateChild<Duin::Sprite2D>("Textures/at_symbol.png");
	sprite->SetTextureSize(50, 50);
}

Duin::Vector2 pos(10, 10);
void Player::PhysicsUpdate(double pDelta)
{
	pos += input * 5;
	SetLocalPosition2D(pos);
}

void Player::HandleInput(Duin::InputEvent e)
{
	input = Duin::Vector2::ZERO;
	input = e.GetInputVector(KEY_W, KEY_S, KEY_A, KEY_D);
}
