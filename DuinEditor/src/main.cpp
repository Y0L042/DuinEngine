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
	void Update(double rDelta) override;
	void PhysicsUpdate(double pDelta) override;
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

void DuinEditor::Update(double rDelta)
{
}

void DuinEditor::PhysicsUpdate(double pDelta)
{
}

void DuinEditor::Draw()
{
}
