#include "PlayerStateOnGroundSprint.h"

#include "./PlayerStates.h"
#include "../ECS.h"

#include "../Singletons.h"
#include "../GameStates/StateGameLoop.h"

#include <Duin/ECS/ECSModule.h>
#include <Duin/Core/Debug/DebugModule.h>
#include <Duin/Core/Maths/MathsModule.h>

PlayerStateOnGroundSprint::PlayerStateOnGroundSprint(duin::GameStateMachine& owner)
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

void PlayerStateOnGroundSprint::State_Enter()
{
    player.add<SprintTag>();
}

void PlayerStateOnGroundSprint::State_HandleInput()
{
    //if (!IsInputVector2DPressedStruct(MOVEMENT_KEYS)) {
    //    owner.SwitchState<PlayerStateOnGroundIdle>();
    //}

    //duin::Vector2 input(GetInputVector2DStruct(MOVEMENT_KEYS));
    //player.set<PlayerMovementInputVec3>({ { input.x, 0.0f, input.y } });

    //if (!duin::Vector2Equals(input, duin::Vector2Zero())) {
    //    if (!IsKeyDown(KEY_LEFT_SHIFT)) {
    //        owner.SwitchState<PlayerStateOnGroundRun>();
    //    }
    //    else {
    //        player.add<SprintTag>();
    //    }
    //}
}

void PlayerStateOnGroundSprint::State_Update(double delta)
{
}

void PlayerStateOnGroundSprint::State_PhysicsUpdate(double delta)
{
}

void PlayerStateOnGroundSprint::State_Draw()
{
}

void PlayerStateOnGroundSprint::State_DrawUI()
{
    debugWatchlist.Post("PlayerState", "OnGroundSprint");
}

void PlayerStateOnGroundSprint::State_Exit()
{
    player.remove<SprintTag>();
    debugWatchlist.Post("PlayerState", "");

    player.remove<SprintTag>();
}
