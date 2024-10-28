#include <Duin.h>
#include <Duin/EntryPoint.h>

#include <memory>

#include "Components/Components.h"
#include "Components/Handlers.h"
#include "Player.h"
#include "PlayerBoids.h"
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

	std::shared_ptr<Duin::AssetManager> assetManager;
	std::shared_ptr<Duin::Registry> registry;
	std::shared_ptr<Handler_PlayerInput> handlerPlayerInput;
	std::shared_ptr<Handler_PlayerMovement> handlerPlayerMovement;
	std::shared_ptr<Handler_UpdateTransform> handlerUpdateTransform;
	std::shared_ptr<Handler_RenderableEntity> handlerRenderableEntity;
	std::shared_ptr<Handler_ProcessBoids> handlerProcessBoids;
	std::shared_ptr<Handler_PlayerBoidMovement> handlerPlayerBoidMovement;
	std::shared_ptr<Handler_BoidsFollowLeader> handlerBoidsFollowLeader;

	std::shared_ptr<Duin::Blackboard> sharedData;
	std::shared_ptr<Duin::QuadTree> quadTree;

	std::shared_ptr<Player> player;
	std::shared_ptr<PlayerBoids> playerBoids;
	std::shared_ptr<AstroidCluster> astroidCluster;
};
Duin::Application* Duin::CreateApplication() { return new Astroids(); }



void Astroids::Initialize()
{
	SetBackgroundColor(Duin::BLACK);
	SetWindowName("Astroids");

	assetManager = std::make_shared<Duin::AssetManager>();
	registry = std::make_shared<Duin::Registry>();
	quadTree = std::make_shared<Duin::QuadTree>(Duin::Rectangle{ 0, 0, 1280, 720 }, 5, 10, 0);

	sharedData = std::make_shared<Duin::Blackboard>();
	sharedData->AddValue("ASSETMGR", assetManager);
	sharedData->AddValue("REG", registry);
	sharedData->AddValue("QTREE", quadTree);

	handlerPlayerInput = std::make_shared<Handler_PlayerInput>(registry);
	handlerPlayerMovement = std::make_shared<Handler_PlayerMovement>(registry);
	handlerUpdateTransform = std::make_shared<Handler_UpdateTransform>(registry, 1280 - 25, 720 - 25);
	handlerRenderableEntity = std::make_shared<Handler_RenderableEntity>(registry);
	handlerProcessBoids = std::make_shared<Handler_ProcessBoids>(registry);
	handlerPlayerBoidMovement = std::make_shared<Handler_PlayerBoidMovement>(registry);
	handlerBoidsFollowLeader = std::make_shared<Handler_BoidsFollowLeader>(registry);
}

void Astroids::Exit()
{

}

void Astroids::Ready()
{
	player = GetSceneManager().CreateNode<Player>(sharedData);
	playerBoids = GetSceneManager().CreateNode<PlayerBoids>(sharedData, player->entity);
	astroidCluster = GetSceneManager().CreateNode<AstroidCluster>(sharedData);
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
	handlerProcessBoids->Update(pDelta);
	handlerPlayerBoidMovement->Update(pDelta);
	handlerBoidsFollowLeader->Update(pDelta);

	handlerUpdateTransform->Update(pDelta);
}

void Astroids::Draw()
{
	handlerRenderableEntity->Update();
}
