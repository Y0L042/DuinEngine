#include <Duin.h>
#include <Duin/EntryPoint.h>

class Astroids : public Duin::Application
{
public:
	Astroids() {}
	~Astroids() {}

	void Initialize() override;
	void Ready() override;
	void HandleInputs(Duin::InputEvent e) override;
	void Process(double rDelta) override;
	void PhysicsProcess(double pDelta) override;
	void Draw() override;


};

void Astroids::Initialize()
{
}

void Astroids::Ready()
{
}

void Astroids::HandleInputs(Duin::InputEvent e)
{
}

void Astroids::Process(double rDelta)
{
}

void Astroids::PhysicsProcess(double pDelta)
{
}

void Astroids::Draw()
{
}
