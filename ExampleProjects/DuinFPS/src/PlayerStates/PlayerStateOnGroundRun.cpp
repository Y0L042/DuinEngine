#include "PlayerStateOnGroundRun.h"

#include "./PlayerStates.h"
#include "../ECS.h"

#include "../Singletons.h"
#include "../GameStates/StateGameLoop.h"

#include <Duin/ECS/ECSModule.h>
#include <Duin/Core/Debug/DebugModule.h>
#include <Duin/Core/Maths/MathsModule.h>
#include <Duin/Core/Events/EventsModule.h>

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
}

void PlayerStateOnGroundRun::State_Update(double delta)
{
}

void PlayerStateOnGroundRun::State_PhysicsUpdate(double delta)
{
    if (!duin::Input::IsInputVectorPressed(DN_KEY_W, DN_KEY_S, DN_KEY_A, DN_KEY_D)) {
        owner.SwitchState<PlayerStateOnGroundIdle>();
    }

    duin::Vector2 input = duin::Input::GetInputVector(DN_KEY_W, DN_KEY_S, DN_KEY_A, DN_KEY_D);
    player.set<PlayerMovementInputVec3>({ { input.x, 0.0f, input.y } });

    if (!duin::Vector2Equals(input, duin::Vector2Zero())) {
        if (duin::Input::IsKeyPressed(DN_KEY_MOD_LSHIFT) || duin::Input::IsKeyDown(DN_KEY_MOD_LSHIFT)) {
            owner.SwitchState<PlayerStateOnGroundSprint>();
        }
        else {
            player.add<RunTag>();
        }
    }
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
