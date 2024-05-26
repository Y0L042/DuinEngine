#include <Duin.h>
#include <Duin/EntryPoint.h>

#include "Player.h"
#include "Components/PlayerComponent.h"
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

entt::scheduler inputScheduler;
entt::scheduler movementScheduler;
entt::scheduler renderScheduler;

uint32_t cheatDelta = 0;


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
	entity->AddComponent<TransformCMP>();
	entity->AddComponent<MovementCMP>();
	entity->AddComponent<RenderableCMP>(new Duin::TextureRes("Textures/at_symbol.png"), Duin::Vector2{ 15, 15 });

	inputScheduler.attach<PlayerCMPManager>(Duin::Registry::GetRegistry());
	movementScheduler.attach<MovementCMPManager>(Duin::Registry::GetRegistry());
	movementScheduler.attach<TransformCMPManager>(Duin::Registry::GetRegistry());
	renderScheduler.attach<RenderableCMPManager>(Duin::Registry::GetRegistry());
}

void Sandbox::Ready()
{

}

void Sandbox::HandleInputs(Duin::InputEvent e)
{
	auto view = Duin::Registry::GetRegistry().view<PlayerCMP, MovementCMP>();
	for (auto [entity, movement] : view.each())
	{
		movement.velocity = e.GetInputVector(KEY_W, KEY_S, KEY_A, KEY_D);
	}
	//inputScheduler.update(cheatDelta);
}

void Sandbox::Process(double rDelta)
{

}

void Sandbox::PhysicsProcess(double pDelta)
{
	cheatDelta = (uint32_t)pDelta;
	movementScheduler.update(cheatDelta);
}

void Sandbox::Draw()
{
	renderScheduler.update(cheatDelta);
}
