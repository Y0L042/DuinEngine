#include <Duin.h>
#include <Duin/EntryPoint.h>




int x = 0;

Duin::TextureRes texture1;
Duin::TextureRes texture2;

class Sandbox : public Duin::Application
{
public:
	Sandbox() {}
	~Sandbox() {}

	void Ready() override;
	void HandleInputs(Duin::InputEvent e) override;
	void Process(double rDelta) override;
	void PhysicsProcess(double pDelta) override;
	void Draw() override;
};
Duin::Application* Duin::CreateApplication() { return new Sandbox(); }



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
	texture1.Draw(0, 0);
}
