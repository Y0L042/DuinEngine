#pragma once

#include <Duin.h>

class Player : public Duin::Node2D
{
public:
	Player();
	~Player();

	void Initialize() override;
	void PhysicsUpdate(double pDelta) override;

private:
	std::shared_ptr<Duin::Sprite2D> sprite;

};