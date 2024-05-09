#include <Duin.h>
#include <Duin/EntryPoint.h>




int x = 0;

Duin::Texture texture1;
Duin::Texture texture2;

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
	texture1.LoadTexture("Textures/duin.bmp");
	texture2.LoadTexture("Textures/duin.bmp");
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
	texture1.Draw(0, 0, 250, 250);
	texture2.Draw(x, 350, 200, 200);
}
