#include <Duin.h>
#include <Duin/EntryPoint.h>

#include <memory>

#include "Components/AllComponents.h"
#include "Player.h"

class Astroids : public Duin::Application
{
public:
	Astroids() {}
	~Astroids() {}

	void Initialize() override;
	void Exit() override;
	void Ready() override;
	void HandleInputs(Duin::InputEvent e) override;
	void Process(double rDelta) override;
	void PhysicsProcess(double pDelta) override;
	void Draw() override;

	Duin::Registry* registry;
	Handler_PlayerInput* handlerPlayerInput;
	Handler_PlayerMovement* handlerPlayerMovement;
	Handler_UpdateTransform* handlerUpdateTransform;
	Handler_RenderableEntity* handlerRenderableEntity;
	std::shared_ptr<Player> player;
};
Duin::Application* Duin::CreateApplication() { return new Astroids(); }

void Astroids::Initialize()
{
	registry = new Duin::Registry();
	handlerPlayerInput = new Handler_PlayerInput(registry);
	handlerPlayerMovement = new Handler_PlayerMovement(registry);
	handlerUpdateTransform = new Handler_UpdateTransform(registry, 1280 - 25, 720 - 25);
	handlerRenderableEntity = new Handler_RenderableEntity(registry);
}

void Astroids::Exit()
{
	delete registry;
	delete handlerPlayerInput;
	delete handlerPlayerMovement;
	delete handlerUpdateTransform;
	delete handlerRenderableEntity;
}

void Astroids::Ready()
{
	player = std::make_shared<Player>(registry);
}

void Astroids::HandleInputs(Duin::InputEvent e)
{
	handlerPlayerInput->Update(e);
}

void Astroids::Process(double rDelta)
{
}

void Astroids::PhysicsProcess(double pDelta)
{
	handlerPlayerMovement->Update();

	handlerUpdateTransform->Update(pDelta);
}

void Astroids::Draw()
{
	handlerRenderableEntity->Update();
}
