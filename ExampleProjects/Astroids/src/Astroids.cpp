#include <Duin.h>
#include <Duin/EntryPoint.h>

#include <memory>

#include "Components/Component_Movement.h"
#include "Components/Component_PlayerInput.h"
#include "Components/Component_Transform.h"
#include "Components/Component_Renderable.h"
#include "Components/Handler_PlayerInput.h"
#include "Components/Handler_PlayerMovement.h"
#include "Components/Handler_UpdateTransform.h"
#include "Components/Handler_Renderable.h"

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
	Handler_Renderable* handlerRenderable;
	std::shared_ptr<Duin::Entity> player;

	void CreatePlayer();
};
Duin::Application* Duin::CreateApplication() { return new Astroids(); }

void Astroids::Initialize()
{
	registry = new Duin::Registry();
	handlerPlayerInput = new Handler_PlayerInput(registry);
	handlerPlayerMovement = new Handler_PlayerMovement(registry);
	handlerUpdateTransform = new Handler_UpdateTransform(registry, 1280 - 25, 720 - 25);
	handlerRenderable = new Handler_Renderable(registry);
	CreatePlayer();
}

void Astroids::Exit()
{
	delete registry;
	delete handlerPlayerInput;
	delete handlerPlayerMovement;
	delete handlerUpdateTransform;
	delete handlerRenderable;
}

void Astroids::Ready()
{
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
	handlerRenderable->Update();
}

void Astroids::CreatePlayer()
{
	std::shared_ptr<Duin::TextureResource> texture = std::make_shared<Duin::TextureResource>("assets/spaceship.png");
	player = Duin::Entity::Create(registry);
	player->AddComponent<Component_PlayerInput>();
	player->AddComponent<Component_Movement>();
	player->AddComponent<Component_Transform>();
	player->AddComponent<Component_Renderable>(texture, Duin::Vector2{ 25, 25 });
}
