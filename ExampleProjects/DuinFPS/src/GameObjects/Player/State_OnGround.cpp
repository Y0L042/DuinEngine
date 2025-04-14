#include "State_OnGround.h"
#include "States.h"

#include "../../Singletons.h"
#include "../../ECS.h"

#include <Duin/Core/Utils/UtilsModule.h>
#include <Duin/ECS/ECSModule.h>

using namespace duin::ECSComponent;
using namespace duin::ECSTag;

duin::StateMachine onGroundStateMachine;

void State_OnGround::Enter()
{
	debugConsole.Log("State_OnGround: Entering State_OnGround");

    player.add<OnGroundTag>();
    player.remove<CanGravity>();
    onGroundStateMachine.SwitchState<State_OnGround_Idle>();
}

void State_OnGround::OnEvent(duin::Event e)
{
    if (duin::Input::IsKeyPressed(DN_KEY_SPACE)) {
       player.add<JumpTag>();
    }

    onGroundStateMachine.ExecuteOnEvent(e);
}

void State_OnGround::Update(double delta)
{
	onGroundStateMachine.ExecuteUpdate(delta);
}

void State_OnGround::PhysicsUpdate(double delta)
{
    const CharacterBodyComponent *cbc = player.get<CharacterBodyComponent>(); 
    if (cbc) {
        int isOnFloor = cbc->characterBody->IsOnFloor();
        if (!isOnFloor) {
           SwitchState<State_InAir>();
           return;
        }
    }

	onGroundStateMachine.ExecutePhysicsUpdate(delta);
}

void State_OnGround::Draw()
{
	onGroundStateMachine.ExecuteDraw();
}

void State_OnGround::DrawUI()
{
    debugWatchlist.Post("PlayerIsOnFloor: ", "%d", 1);

	onGroundStateMachine.ExecuteDrawUI();
}

void State_OnGround::Exit()
{
    player.remove<OnGroundTag>();
    onGroundStateMachine.FlushStack();
}

