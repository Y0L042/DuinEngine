#include "PlayerStateInAirStrafe.h"
#include "./PlayerStates.h"

#include "../Singletons.h"

PlayerStateInAirStrafe::PlayerStateInAirStrafe(duin::GameStateMachine& owner)
	: GameState(owner)
{
    Enter = [this]() { State_Enter(); };
    Exit = [this]() { State_Exit(); };
    HandleInput = [this]() { State_HandleInput(); };
    Update = [this](double delta) { State_Update(delta); };
    PhysicsUpdate = [this](double delta) { State_PhysicsUpdate(delta); };
    Draw = [this]() { State_Draw(); };
    DrawUI = [this]() { State_DrawUI(); };
}

void PlayerStateInAirStrafe::State_Enter()
{
}

void PlayerStateInAirStrafe::State_HandleInput()
{
}

void PlayerStateInAirStrafe::State_Update(double delta)
{
}

void PlayerStateInAirStrafe::State_PhysicsUpdate(double delta)
{
}

void PlayerStateInAirStrafe::State_Draw()
{
}

void PlayerStateInAirStrafe::State_DrawUI()
{
    debugWatchlist.Post("PlayerState", "InAirStrafe");
}

void PlayerStateInAirStrafe::State_Exit()
{
    debugWatchlist.Post("PlayerState", "");
}
