#pragma once

#include <Duin.h>

class Sandbox : public Duin::Application
{
public:
	Sandbox() {}
	~Sandbox() {}

	void Initialize() override;
	void Ready() override;
	void HandleInputs(Duin::InputEvent e) override;
	void Process(double rDelta) override;
	void PhysicsProcess(double pDelta) override;
	void Draw() override;

	void SpawnEntityBatches();
	void SpawnEntity(Duin::Vector2 position, Duin::Vector2 velocity);

};