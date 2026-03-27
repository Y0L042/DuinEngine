#include "State_OnGround_Idle.h"
#include "States.h"

#include "../../Singletons.h"
#include "../../ECS.h"

#include <Duin/Core/Utils/UtilsModule.h>
#include <Duin/ECS/ECSModule.h>

using namespace duin::ECSComponent;
using namespace duin::ECSTag;

void State_OnGround_Idle::Enter()
{
    debugConsole.Log("State_OnGround_Idle: Entering State_OnGround_Idle");

    GetPlayer().Add<IdleTag>();
}

void State_OnGround_Idle::OnEvent(duin::Event e)
{
}

void State_OnGround_Idle::Update(double delta)
{
}

void State_OnGround_Idle::PhysicsUpdate(double delta)
{
    if (IsMovementInputPressed())
    {
        SwitchState<State_OnGround_Run>();
    }
}

void State_OnGround_Idle::Draw()
{
}

void State_OnGround_Idle::DrawUI()
{
    debugWatchlist.Post("PlayerState", "OnGroundIdle");
}

void State_OnGround_Idle::Exit()
{
    GetPlayer().Remove<IdleTag>();
    debugWatchlist.Post("PlayerState", "");
}
