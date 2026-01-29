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

    player.add<IdleTag>();
}

void State_OnGround_Idle::OnEvent(duin::Event e)
{
}

void State_OnGround_Idle::Update(double delta)
{
}

void State_OnGround_Idle::PhysicsUpdate(double delta)
{
    if (duin::Input::IsInputVectorPressed(DN_KEY_W, DN_KEY_S, DN_KEY_A, DN_KEY_D))
    {
        // if (duin::Input::IsKeyPressed(DN_KEY_MOD_LSHIFT)) {
        //     owner.SwitchState<PlayerStateOnGroundSprint>();
        // } else {
        SwitchState<State_OnGround_Run>();
        // }
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
    player.remove<IdleTag>();
    debugWatchlist.Post("PlayerState", "");
}
