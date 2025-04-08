#include "PlayerStateInAirStrafe.h"
#include "./PlayerStates.h"

#include "../ECS.h"
#include "../Singletons.h"
#include "../GameStates/StateGameLoop.h"

#include <Duin/ECS/ECSModule.h>
#include <Duin/Core/Debug/DebugModule.h>
#include <Duin/Core/Maths/MathsModule.h>
#include <Duin/Core/Events/EventsModule.h>

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
    player.add<RunTag>();
}

void PlayerStateInAirStrafe::State_HandleInput()
{
    if (!duin::Input::IsInputVectorPressed(DN_KEY_W, DN_KEY_S, DN_KEY_A, DN_KEY_D)) {
        owner.SwitchState<PlayerStateInAirIdle>();
    }

    duin::Vector2 input = duin::Input::GetInputVector(DN_KEY_W, DN_KEY_S, DN_KEY_A, DN_KEY_D);
    player.set<PlayerMovementInputVec3>({ { input.x, 0.0f, input.y } });
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
    player.remove<RunTag>();
    debugWatchlist.Post("PlayerState", "");
}
