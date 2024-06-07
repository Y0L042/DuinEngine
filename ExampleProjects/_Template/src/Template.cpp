#include <Duin.h>
#include <Duin/EntryPoint.h>

class Template : public Duin::Application
{
public:
	Template() {}
	~Template() {}

	void Initialize() override;
	void Ready() override;
	void HandleInputs(Duin::InputEvent e) override;
	void Process(double rDelta) override;
	void PhysicsProcess(double pDelta) override;
	void Draw() override;


};

void Template::Initialize()
{
}

void Template::Ready()
{
}

void Template::HandleInputs(Duin::InputEvent e)
{
}

void Template::Process(double rDelta)
{
}

void Template::PhysicsProcess(double pDelta)
{
}

void Template::Draw()
{
}
