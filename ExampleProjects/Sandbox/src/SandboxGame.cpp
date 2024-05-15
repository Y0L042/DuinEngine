#include <Duin.h>
#include <Duin/EntryPoint.h>

#include "Player.h"


int x = 0;

Duin::TextureRes texture1;
Duin::TextureRes texture2;

std::shared_ptr<Player> player;

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
	player = InstantiateChild<Player>();
}

void Sandbox::Ready()
{
	texture1.LoadTexture("Textures/at_symbol.png");
	texture1.SetTextureSize(100.0f, 100.0f);
}

void Sandbox::HandleInputs(Duin::InputEvent e)
{
	
}

void Sandbox::Process(double rDelta)
{

}

void Sandbox::PhysicsProcess(double pDelta)
{
	x++;
}

void Sandbox::Draw()
{
	texture1.Draw(100, 100);
}
