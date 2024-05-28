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

#include <random>

int min = 20;
int max = 700;
std::random_device rd;  // Obtain a random number from hardware
std::mt19937 gen(rd()); // Seed the generator
std::uniform_int_distribution<> distr(min, max); // Define the range

int x = 0;

Duin::TextureRes texture1;
Duin::TextureRes texture2;

std::shared_ptr<Duin::Node> root;
std::shared_ptr<Player> player;
std::shared_ptr<Duin::TextureRes> texture;

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

	std::shared_ptr<Duin::Entity> entity;
	texture = std::make_shared<Duin::TextureRes>("Textures/at_symbol.png");
	for (int i = 0; i < 10000; i++)
	{
		entity = Duin::Entity::CreatePointer();
		entity->AddComponent<PlayerCMP>();
		entity->AddComponent<PlayerInputCMP>();
		entity->AddComponent<MovementCMP>();
		entity->AddComponent<TransformCMP>(Duin::Vector2{ (float)distr(gen), (float)distr(gen) });
		entity->AddComponent<RenderableCMP>(texture.get(), Duin::Vector2{15, 15});
	}
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
