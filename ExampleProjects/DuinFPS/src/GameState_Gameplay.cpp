#include "GameState_Gameplay.h"

#include "Singletons.h"
#include "GameObjects/Player/Player.h"

#include <Duin/ECS/ECSModule.h>

std::shared_ptr<Player> player;

void GameState_Gameplay::Enter()
{
	debugConsole.Log("GameState_Gameplay: Entering GameState_Gameplay");
	player = CreateChild<Player>();
}

void GameState_Gameplay::OnEvent(duin::Event e)
{
}

void GameState_Gameplay::Update(double delta)
{
}

void GameState_Gameplay::PhysicsUpdate(double delta)
{
}

void GameState_Gameplay::Draw()
{
}

void GameState_Gameplay::DrawUI()
{
}

void GameState_Gameplay::Exit()
{
}
