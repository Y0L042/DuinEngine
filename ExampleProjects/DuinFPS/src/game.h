#pragma once

#include <Duin/Core/Application.h>
#include <Duin/Core/Events/EventsModule.h>

class Game : public duin::Application
{
public:
	Game() {}
	~Game() {}

	void Initialize() override;
	void Ready() override;
	void HandleInputs(duin::InputEvent event) override;
	void Update(double rDelta) override;
	void PhysicsUpdate(double pDelta) override;
	void Draw() override;
    void DrawUI() override;
};

