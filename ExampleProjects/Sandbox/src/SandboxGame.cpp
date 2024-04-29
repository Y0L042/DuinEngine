#include <Duin.h>

class Sandbox : public Duin::Application
{
public:
	Sandbox() {}
	~Sandbox() {}

	void Ready() override;
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
