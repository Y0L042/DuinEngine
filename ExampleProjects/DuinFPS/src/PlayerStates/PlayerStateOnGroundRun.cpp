#include "PlayerStateOnGroundRun.h"

#include "./PlayerStates.h"
#include "../ECS.h"

#include "../Singletons.h"
#include "../GameStates/StateGameLoop.h"

PlayerStateOnGroundRun::PlayerStateOnGroundRun(duin::GameStateMachine& owner)
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

void PlayerStateOnGroundRun::State_Enter()
{
    player.add<RunTag>();
}

void PlayerStateOnGroundRun::State_HandleInput()
{
    //if (!IsInputVector2DPressedStruct(MOVEMENT_KEYS)) {
    //    owner.SwitchState<PlayerStateOnGroundIdle>();
    //}

    //duin::Vector2 input(GetInputVector2DStruct(MOVEMENT_KEYS));
    //player.set<PlayerMovementInputVec3>({ { input.x, 0.0f, input.y } });

    //if (!duin::Vector2Equals(input, duin::Vector2Zero())) {
    //    if (IsKeyPressed(KEY_LEFT_SHIFT) || IsKeyDown(KEY_LEFT_SHIFT)) {
    //        owner.SwitchState<PlayerStateOnGroundSprint>();
    //    }
    //    else {
    //        player.add<RunTag>();
    //    }
    //}
}

void PlayerStateOnGroundRun::State_Update(double delta)
{
}

void PlayerStateOnGroundRun::State_PhysicsUpdate(double delta)
{

}

void PlayerStateOnGroundRun::State_Draw()
{
}

void PlayerStateOnGroundRun::State_DrawUI()
{
    debugWatchlist.Post("PlayerState", "OnGroundRun");
}

void PlayerStateOnGroundRun::State_Exit()
{
    player.remove<RunTag>();
    debugWatchlist.Post("PlayerState", "");
}
