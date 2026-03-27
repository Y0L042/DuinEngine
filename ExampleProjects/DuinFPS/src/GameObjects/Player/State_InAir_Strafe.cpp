#include "State_InAir_Strafe.h"
#include "States.h"

#include "../../Singletons.h"
#include "../../ECS.h"

#include <Duin/Core/Utils/UtilsModule.h>
#include <Duin/ECS/ECSModule.h>

using namespace duin::ECSComponent;
using namespace duin::ECSTag;

void State_InAir_Strafe::Enter()
{
    debugConsole.Log("State_InAir_Strafe: Entering State_InAir_Strafe");

    GetPlayer().Add<RunTag>();
}

void State_InAir_Strafe::OnEvent(duin::Event e)
{
}

void State_InAir_Strafe::Update(double delta)
{
}

void State_InAir_Strafe::PhysicsUpdate(double delta)
{
    duin::Entity& player = GetPlayer();

    if (IsMovementInputPressed())
    {
        SwitchState<State_InAir_Idle>();
    }

    duin::Vector2 input = GetMovementInput();
    player.Set<PlayerMovementInputVec3>({{input.x, 0.0f, input.y}});
}

void State_InAir_Strafe::Draw()
{
}

void State_InAir_Strafe::DrawUI()
{
    debugWatchlist.Post("PlayerState", "InAirStrafe");
}

void State_InAir_Strafe::Exit()
{
    GetPlayer().Remove<RunTag>();
    debugWatchlist.Post("PlayerState", "");
}
