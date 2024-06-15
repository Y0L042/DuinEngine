#include <Duin.h>
#include <Duin/EntryPoint.h>

#include <memory>

#include "Components/Components.h"
#include "Components/Handlers.h"
#include "Player.h"
#include "AstroidCluster.h"

class Astroids : public Duin::Application
{
public:
	Astroids() {}
	~Astroids() {}

	void Initialize() override;
	void Exit() override;
	void Ready() override;
	void HandleInputs(Duin::InputEvent e) override;
	void Update(double rDelta) override;
	void PhysicsUpdate(double pDelta) override;
	void Draw() override;

	Duin::AssetManager* assetManager;
	Duin::Registry* registry;
	Handler_PlayerInput* handlerPlayerInput;
	Handler_PlayerMovement* handlerPlayerMovement;
	Handler_UpdateTransform* handlerUpdateTransform;
	Handler_RenderableEntity* handlerRenderableEntity;

	std::shared_ptr<Player> player;
	std::shared_ptr<AstroidCluster> astroidCluster;
};
Duin::Application* Duin::CreateApplication() { return new Astroids(); }

void Astroids::Initialize()
{
	SetBackgroundColor(Duin::BLACK);
	SetWindowName("Astroids");

	assetManager = new Duin::AssetManager();

	registry = new Duin::Registry();
	handlerPlayerInput = new Handler_PlayerInput(registry);
	handlerPlayerMovement = new Handler_PlayerMovement(registry);
	handlerUpdateTransform = new Handler_UpdateTransform(registry, 1280 - 25, 720 - 25);
	handlerRenderableEntity = new Handler_RenderableEntity(registry);
}

void Astroids::Exit()
{
	delete assetManager;
	delete registry;
	delete handlerPlayerInput;
	delete handlerPlayerMovement;
	delete handlerUpdateTransform;
	delete handlerRenderableEntity;
}

void Astroids::Ready()
{
	player = GetSceneManager().CreateNode<Player>(registry, assetManager);
	astroidCluster = GetSceneManager().CreateNode<AstroidCluster>(registry, assetManager);
}

void Astroids::HandleInputs(Duin::InputEvent e)
{
	handlerPlayerInput->Update(e);
}

void Astroids::Update(double rDelta)
{
}

void Astroids::PhysicsUpdate(double pDelta)
{
	handlerPlayerMovement->Update(pDelta);

	handlerUpdateTransform->Update(pDelta);
}

void Astroids::Draw()
{
	handlerRenderableEntity->Update();
}
