#include "State_InAir.h"
#include "States.h"

#include "../../Singletons.h"
#include "../../ECS.h"

#include <Duin/Core/Utils/UtilsModule.h>
#include <Duin/ECS/ECSModule.h>

using namespace duin::ECSComponent;
using namespace duin::ECSTag;

std::shared_ptr<PlayerStateMachine> inAirStateMachine;

void State_InAir::Enter()
{
    debugConsole.Log("State_InAir: Entering State_InAir");

    duin::Entity& player = *GetBlackboard()->player;

    player.Add<InAirTag>();
    player.Add<CanGravity>();
    inAirStateMachine = CreateChildObject<PlayerStateMachine>(GetBlackboard());
    inAirStateMachine->SwitchState<State_InAir_Idle>();
}

void State_InAir::OnEvent(duin::Event e)
{
}

void State_InAir::Update(double delta)
{
}

void State_InAir::PhysicsUpdate(double delta)
{
    duin::Entity& player = *GetBlackboard()->player;

    int isOnFloor = 0;

    const CharacterBodyComponent *bodyComponent = player.TryGet<CharacterBodyComponent>();
    if (bodyComponent)
    {
        isOnFloor = bodyComponent->body->IsOnFloor();
    }

    if (isOnFloor)
    {
        SwitchState<State_OnGround>();
        return;
    }

}

void State_InAir::Draw()
{
}

void State_InAir::DrawUI()
{
    debugWatchlist.Post("PlayerIsOnFloor: ", "%d", 0);

}

void State_InAir::Exit()
{
    debugConsole.Log("State_InAir: Exiting State_InAir");

    duin::Entity& player = *GetBlackboard()->player;

    player.Remove<InAirTag>();
}
