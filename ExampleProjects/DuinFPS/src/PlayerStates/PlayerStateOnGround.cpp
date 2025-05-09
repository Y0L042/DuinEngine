#include "PlayerStateOnGround.h"
#include "./PlayerStates.h"

#include "../Singletons.h"
#include "../ECS.h"

#include <Duin/ECS/ECSModule.h>
#include <Duin/Core/Debug/DebugModule.h>
#include <Duin/Core/Maths/MathsModule.h>
#include <Duin/Physics/PhysicsModule.h>
#include <Duin/Core/Events/EventsModule.h>

using namespace duin::ECSComponent;
using namespace duin::ECSTag;

PlayerStateOnGround::PlayerStateOnGround(duin::GameStateMachine& owner)
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

void PlayerStateOnGround::State_Enter()
{
    player.add<OnGroundTag>();
    player.remove<CanGravity>();
    debugWatchlist.Post("PlayerIsOnFloor: ", "%d", 1);
    onGroundSM.SwitchState<PlayerStateOnGroundIdle>();
}

void PlayerStateOnGround::State_HandleInput()
{
    if (duin::Input::IsKeyPressed(DN_KEY_SPACE)) {
       player.add<JumpTag>();
    }

    onGroundSM.ExecuteHandleInput();
}

void PlayerStateOnGround::State_Update(double delta)
{
    onGroundSM.ExecuteUpdate(delta);
}

void PlayerStateOnGround::State_PhysicsUpdate(double delta)
{
    const CharacterBodyComponent *cbc = player.get<CharacterBodyComponent>(); 
    if (cbc) {
        int isOnFloor = cbc->characterBody->IsOnFloor();
        if (!isOnFloor) {
           playerSM.SwitchState<PlayerStateInAir>();
           return;
        }
    }

    onGroundSM.ExecutePhysicsUpdate(delta);
}

void PlayerStateOnGround::State_Draw()
{
    onGroundSM.ExecuteDraw();
}

void PlayerStateOnGround::State_DrawUI()
{
    onGroundSM.ExecuteDrawUI();
}

void PlayerStateOnGround::State_Exit()
{
    player.remove<OnGroundTag>();
    onGroundSM.FlushStack();
}
