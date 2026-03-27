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

    GetPlayer().Add<RunTag>();
}

void State_OnGround_Run::OnEvent(duin::Event e)
{
}

void State_OnGround_Run::Update(double delta)
{
}

void State_OnGround_Run::PhysicsUpdate(double delta)
{
    duin::Entity& player = GetPlayer();

    if (!IsMovementInputPressed())
    {
        SwitchState<State_OnGround_Idle>();
    }

    duin::Vector2 input = GetMovementInput();
    player.Set<PlayerMovementInputVec3>({{input.x, 0.0f, input.y}});

    if (!duin::Vector2Equals(input, duin::Vector2Zero()))
    {
        player.Add<RunTag>();
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
    GetPlayer().Remove<RunTag>();
    debugWatchlist.Post("PlayerState", "");
}
