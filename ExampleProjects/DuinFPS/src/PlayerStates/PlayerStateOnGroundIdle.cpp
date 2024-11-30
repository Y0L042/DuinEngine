#include "PlayerStateOnGroundIdle.h"
#include "./PlayerStates.h"

#include "../ECS.h"
#include "../Singletons.h"
#include "../GameStates/StateGameLoop.h"

extern InputVector2DKeys MOVEMENT_KEYS;

PlayerStateOnGroundIdle::PlayerStateOnGroundIdle(duin::GameStateMachine& owner)
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

void PlayerStateOnGroundIdle::State_Enter()
{
    player.add<OnGroundIdleTag>();
}

void PlayerStateOnGroundIdle::State_HandleInput()
{
    if (IsInputVector2DPressedStruct(MOVEMENT_KEYS)) {
        if (IsKeyPressed(KEY_LEFT_SHIFT)) {
            owner.SwitchState<PlayerStateOnGroundSprint>();
        } else {
            owner.SwitchState<PlayerStateOnGroundRun>();
        }
    }
}

void PlayerStateOnGroundIdle::State_Update(double delta)
{
}

void PlayerStateOnGroundIdle::State_PhysicsUpdate(double delta)
{
}

void PlayerStateOnGroundIdle::State_Draw()
{
}

void PlayerStateOnGroundIdle::State_DrawUI()
{
    debugWatchlist.Post("PlayerState", "OnGroundIdle");
}

void PlayerStateOnGroundIdle::State_Exit()
{
    debugWatchlist.Post("PlayerState", "");
}
