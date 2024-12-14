#include "PlayerStateInAirIdle.h"
#include "./PlayerStates.h"

#include "../ECS.h"
#include "../Singletons.h"
#include "../GameStates/StateGameLoop.h"

PlayerStateInAirIdle::PlayerStateInAirIdle(duin::GameStateMachine& owner)
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

void PlayerStateInAirIdle::State_Enter()
{
    player.add<IdleTag>();
}

void PlayerStateInAirIdle::State_HandleInput()
{
    if (IsInputVector2DPressedStruct(MOVEMENT_KEYS)) {
        owner.SwitchState<PlayerStateInAirStrafe>();
    }
}

void PlayerStateInAirIdle::State_Update(double delta)
{
}

void PlayerStateInAirIdle::State_PhysicsUpdate(double delta)
{
}

void PlayerStateInAirIdle::State_Draw()
{
}

void PlayerStateInAirIdle::State_DrawUI()
{
    debugWatchlist.Post("PlayerState", "InAirIdle");
}

void PlayerStateInAirIdle::State_Exit()
{
    player.remove<IdleTag>();
    debugWatchlist.Post("PlayerState", "");
}
