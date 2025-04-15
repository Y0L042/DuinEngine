#include "State_InAir.h"
#include "States.h"

#include "../../Singletons.h"
#include "../../ECS.h"

#include <Duin/Core/Utils/UtilsModule.h>
#include <Duin/ECS/ECSModule.h>

using namespace duin::ECSComponent;
using namespace duin::ECSTag;

duin::StateMachine inAirStateMachine;

void State_InAir::Enter()
{
	debugConsole.Log("State_InAir: Entering State_InAir");

    player.add<InAirTag>();
    player.add<CanGravity>();
    inAirStateMachine.SwitchState<State_InAir_Idle>();
}

void State_InAir::OnEvent(duin::Event e)
{
    inAirStateMachine.ExecuteOnEvent(e);
}

void State_InAir::Update(double delta)
{
    inAirStateMachine.ExecuteUpdate(delta);
}

void State_InAir::PhysicsUpdate(double delta)
{
    int isOnFloor = 0;

    const CharacterBodyComponent *bodyComponent = player.get<CharacterBodyComponent>();
    if (bodyComponent) {
        isOnFloor = bodyComponent->body->IsOnFloor();
    }
    
    if (isOnFloor) {
        SwitchState<State_OnGround>();
        return;
    }

    inAirStateMachine.ExecutePhysicsUpdate(delta);
}

void State_InAir::Draw()
{
    inAirStateMachine.ExecuteDraw();
}

void State_InAir::DrawUI()
{
    debugWatchlist.Post("PlayerIsOnFloor: ", "%d", 0);

    inAirStateMachine.ExecuteDrawUI();
}

void State_InAir::Exit()
{   
	debugConsole.Log("State_InAir: Exiting State_InAir");

    player.remove<InAirTag>();
    inAirStateMachine.FlushStack();
}

