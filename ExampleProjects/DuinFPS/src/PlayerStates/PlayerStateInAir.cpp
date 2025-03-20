#include "PlayerStateInAir.h"
#include "./PlayerStates.h"

#include "../Singletons.h"
#include "../ECS.h"

using namespace duin::ECSComponent;
using namespace duin::ECSTag;

PlayerStateInAir::PlayerStateInAir(duin::GameStateMachine& owner)
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

void PlayerStateInAir::State_Enter()
{
    player.add<InAirTag>();
    player.add<CanGravity>();
    debugWatchlist.Post("PlayerIsOnFloor: ", "%d", 0);
    inAirSM.SwitchState<PlayerStateInAirIdle>();
}

void PlayerStateInAir::State_HandleInput()
{
    inAirSM.ExecuteHandleInput();
}

void PlayerStateInAir::State_Update(double delta)
{
    inAirSM.ExecuteUpdate(delta);
}

void PlayerStateInAir::State_PhysicsUpdate(double delta)
{
    int isOnFloor = 0;

    const CharacterBodyComponent *bodyComponent = player.get<CharacterBodyComponent>();
    if (bodyComponent) {
        isOnFloor = bodyComponent->characterBody->IsOnFloor();
    }
    
    if (isOnFloor) {
        playerSM.SwitchState<PlayerStateOnGround>();
        return;
    }

    inAirSM.ExecutePhysicsUpdate(delta);
}

void PlayerStateInAir::State_Draw()
{
    inAirSM.ExecuteDraw();
}

void PlayerStateInAir::State_DrawUI()
{
    inAirSM.ExecuteDrawUI();
}

void PlayerStateInAir::State_Exit()
{
    player.remove<InAirTag>();
    inAirSM.FlushStack();
}
