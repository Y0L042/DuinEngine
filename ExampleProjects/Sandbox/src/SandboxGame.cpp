#include <Duin.h>

class Sandbox : public Duin::Application
{
public:
	Sandbox() {}
	~Sandbox() {}

	void Ready() override;
	void HandleEvents(SDL_Event& e);
	void Process(double rDelta) override;
	void PhysicsProcess(double pDelta) override;
	void Draw() override;
};

Duin::Application* Duin::CreateApplication()
{
	return new Sandbox();
}




void Sandbox::Ready()
{
	Duin::Texture texture;
	std::string("Textures/duin.bmp");
	texture.LoadTexture();
}

void Sandbox::HandleEvents(SDL_Event& e)
{
	
}

void Sandbox::Process(double rDelta)
{

}

void Sandbox::PhysicsProcess(double pDelta)
{
}

void Sandbox::Draw()
{
}
