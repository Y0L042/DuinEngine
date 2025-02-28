#include "PlayerStateOnGround.h"
#include "./PlayerStates.h"

#include "../Singletons.h"
#include "../ECS.h"

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
    player.remove<CanGravity>();
    onGroundSM.SwitchState<PlayerStateOnGroundIdle>();
}

void PlayerStateOnGround::State_HandleInput()
{
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
    onGroundSM.FlushStack();
}
