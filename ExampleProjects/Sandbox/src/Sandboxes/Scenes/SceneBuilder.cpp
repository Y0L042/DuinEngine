#include "SceneBuilder.h"
#include <Duin/Core/Debug/DNLog.h>
#include <Duin/Core/Debug/DNAssert.h>

#include <string>

SceneBuilder::SceneBuilder(duin::GameStateMachine &sm) : duin::GameState(sm)
{
}

SceneBuilder::~SceneBuilder()
{
}

void SceneBuilder::Enter()
{
}

void SceneBuilder::OnEvent(duin::Event e)
{
}

void SceneBuilder::Update(double delta)
{
}

void SceneBuilder::PhysicsUpdate(double delta)
{
}

void SceneBuilder::Draw()
{
}

void SceneBuilder::DrawUI()
{
}

void SceneBuilder::Exit()
{
}

void SceneBuilder::SetPause()
{
}

void SceneBuilder::SetUnpause()
{
}

void SceneBuilder::Tests()
{
}

void SceneBuilder::Test_Misc()
{
}
