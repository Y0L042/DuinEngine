#include <Duin.h>
#include <Duin/EntryPoint.h>

#include "Player.h"
#include "Components/PlayerComponent.h"
#include "Components/PlayerInputComponent.h"
#include "Components/PlayerCMPManager.h"
#include "Components/MovementComponent.h"
#include "Components/MovementCMPManager.h"
#include "Components/TransformComponent.h"
#include "Components/TransformCMPManager.h"
#include "Components/RenderableComponent.h"
#include "Components/RenderableCMPManager.h"

int x = 0;

Duin::TextureRes texture1;
Duin::TextureRes texture2;

std::shared_ptr<Duin::Node> root;
std::shared_ptr<Player> player;
std::shared_ptr<Duin::Entity> entity;

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
};
Duin::Application* Duin::CreateApplication() { return new Sandbox(); }



void Sandbox::Initialize()
{
	root = Duin::ObjectManager::GetRootNode();
	player = root->InstantiateChild<Player>();

	entity = Duin::Entity::CreatePointer();
	entity->AddComponent<PlayerCMP>();
	entity->AddComponent<PlayerInputCMP>();
	entity->AddComponent<MovementCMP>();
	entity->AddComponent<TransformCMP>();
	entity->AddComponent<RenderableCMP>(new Duin::TextureRes("Textures/at_symbol.png"), Duin::Vector2{ 15, 15 });
}

void Sandbox::Ready()
{

}

void Sandbox::HandleInputs(Duin::InputEvent e)
{
	PlayerCMPManager::Update(e);
}

void Sandbox::Process(double rDelta)
{
}

void Sandbox::PhysicsProcess(double pDelta)
{
	MovementCMPManager::Update(pDelta);
	TransformCMPManager::Update(pDelta);
}

void Sandbox::Draw()
{
	RenderableCMPManager::Update();
}
