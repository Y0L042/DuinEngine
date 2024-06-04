#include <Duin.h>
#include <Duin/EntryPoint.h>

class DuinEditor : public Duin::Application
{
public:
	DuinEditor() {}
	~DuinEditor() {}

	void Initialize() override;
	void Ready() override;
	void HandleInputs(Duin::InputEvent e) override;
	void Process(double rDelta) override;
	void PhysicsProcess(double pDelta) override;
	void Draw() override;

};
Duin::Application* Duin::CreateApplication() { return new DuinEditor(); }





void DuinEditor::Initialize()
{
}

void DuinEditor::Ready()
{
}

void DuinEditor::HandleInputs(Duin::InputEvent e)
{
}

void DuinEditor::Process(double rDelta)
{
}

void DuinEditor::PhysicsProcess(double pDelta)
{
}

void DuinEditor::Draw()
{
}
