#include "State_SceneEditor.h"

void State_SceneEditor::Enter()
{
    sceneViewport = CreateChildObject<SceneViewport>();
}

void State_SceneEditor::OnEvent(duin::Event e)
{
}

void State_SceneEditor::Update(double delta)
{
}

void State_SceneEditor::PhysicsUpdate(double delta)
{
}

void State_SceneEditor::Draw()
{

}

void State_SceneEditor::DrawUI()
{
}

void State_SceneEditor::Exit()
{
}
