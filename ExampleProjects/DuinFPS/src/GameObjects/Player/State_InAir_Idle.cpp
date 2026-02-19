#include "State_InAir_Idle.h"
#include "States.h"

#include "../../Singletons.h"
#include "../../ECS.h"

#include <Duin/Core/Utils/UtilsModule.h>
#include <Duin/ECS/ECSModule.h>

using namespace duin::ECSComponent;
using namespace duin::ECSTag;

void State_InAir_Idle::Enter()
{
    debugConsole.Log("State_InAir_Idle: Entering State_InAir_Idle");

    duin::Entity& player = *GetBlackboard()->player;

    player.Add<IdleTag>();
}

void State_InAir_Idle::OnEvent(duin::Event e)
{
}

void State_InAir_Idle::Update(double delta)
{
}

void State_InAir_Idle::PhysicsUpdate(double delta)
{
    if (duin::Input::IsInputVectorPressed(DN_KEY_W, DN_KEY_S, DN_KEY_A, DN_KEY_D))
    {
        SwitchState<State_InAir_Strafe>();
    }
}

void State_InAir_Idle::Draw()
{
}

void State_InAir_Idle::DrawUI()
{
    debugWatchlist.Post("PlayerState", "InAirIdle");
}

void State_InAir_Idle::Exit()
{
    debugConsole.Log("State_InAir_Idle: Exiting State_InAir_Idle");

    duin::Entity& player = *GetBlackboard()->player;

    player.Remove<IdleTag>();
    debugWatchlist.Post("PlayerState", "");
}
