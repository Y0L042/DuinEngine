#include "PlayerStateOnGroundIdle.h"
#include "./PlayerStates.h"

#include "../ECS.h"
#include "../Singletons.h"
#include "../GameStates/StateGameLoop.h"

#include <Duin/ECS/ECSModule.h>
#include <Duin/Core/Debug/DebugModule.h>
#include <Duin/Core/Maths/MathsModule.h>
#include <Duin/Core/Events/EventsModule.h>

//extern InputVector2DKeys MOVEMENT_KEYS;

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
    player.add<IdleTag>();
}

void PlayerStateOnGroundIdle::State_HandleInput()
{
}

void PlayerStateOnGroundIdle::State_Update(double delta)
{
}

void PlayerStateOnGroundIdle::State_PhysicsUpdate(double delta)
{
    if (duin::Input::IsInputVectorPressed(DN_KEY_W, DN_KEY_S, DN_KEY_A, DN_KEY_D)) {
       // if (duin::Input::IsKeyPressed(DN_KEY_MOD_LSHIFT)) {
       //     owner.SwitchState<PlayerStateOnGroundSprint>();
       // } else {
           owner.SwitchState<PlayerStateOnGroundRun>();
       // }
    }
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
    player.remove<IdleTag>();
    debugWatchlist.Post("PlayerState", "");
}
