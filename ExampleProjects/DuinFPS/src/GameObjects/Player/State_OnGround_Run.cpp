#include "State_OnGround_Run.h"
#include "States.h"

#include "../../Singletons.h"
#include "../../ECS.h"

#include <Duin/Core/Utils/UtilsModule.h>
#include <Duin/ECS/ECSModule.h>

using namespace duin::ECSComponent;
using namespace duin::ECSTag;

void State_OnGround_Run::Enter()
{
    debugConsole.Log("State_OnGround_Run: Entering State_OnGround_Run");

    duin::Entity& player = *GetBlackboard()->player;

    player.Add<RunTag>();
}

void State_OnGround_Run::OnEvent(duin::Event e)
{
}

void State_OnGround_Run::Update(double delta)
{
}

void State_OnGround_Run::PhysicsUpdate(double delta)
{
    duin::Entity& player = *GetBlackboard()->player;

    if (!duin::Input::IsInputVectorPressed(DN_KEY_W, DN_KEY_S, DN_KEY_A, DN_KEY_D))
    {
        SwitchState<State_OnGround_Idle>();
    }

    duin::Vector2 input = duin::Input::GetInputVector(DN_KEY_W, DN_KEY_S, DN_KEY_A, DN_KEY_D);
    player.Set<PlayerMovementInputVec3>({{input.x, 0.0f, input.y}});

    if (!duin::Vector2Equals(input, duin::Vector2Zero()))
    {
        if (duin::Input::IsKeyPressed(DN_KEY_MOD_LSHIFT) || duin::Input::IsKeyDown(DN_KEY_MOD_LSHIFT))
        {
            // owner.SwitchState<PlayerStateOnGroundSprint>();
        }
        else
        {
            player.Add<RunTag>();
        }
    }
}

void State_OnGround_Run::Draw()
{
}

void State_OnGround_Run::DrawUI()
{
    debugWatchlist.Post("PlayerState", "OnGroundRun");
}

void State_OnGround_Run::Exit()
{
    duin::Entity& player = *GetBlackboard()->player;

    player.Remove<RunTag>();
    debugWatchlist.Post("PlayerState", "");
}
