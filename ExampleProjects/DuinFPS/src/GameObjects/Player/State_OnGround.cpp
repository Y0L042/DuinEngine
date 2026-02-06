#include "State_OnGround.h"
#include "States.h"

#include "../../Singletons.h"
#include "../../ECS.h"

#include <Duin/Core/Utils/UtilsModule.h>
#include <Duin/ECS/ECSModule.h>

using namespace duin::ECSComponent;
using namespace duin::ECSTag;

std::shared_ptr<duin::GameStateMachine> onGroundStateMachine;

void State_OnGround::Enter()
{
    debugConsole.Log("State_OnGround: Entering State_OnGround");

    player.add<OnGroundTag>();
    player.remove<CanGravity>();
    onGroundStateMachine = CreateChildObject<duin::GameStateMachine>();
    onGroundStateMachine->SwitchState<State_OnGround_Idle>();
}

void State_OnGround::OnEvent(duin::Event e)
{
    if (duin::Input::IsKeyPressed(DN_KEY_SPACE))
    {
        player.add<JumpTag>();
    }

}

void State_OnGround::Update(double delta)
{
}

void State_OnGround::PhysicsUpdate(double delta)
{
    const CharacterBodyComponent *cbc = player.try_get<CharacterBodyComponent>();
    if (cbc)
    {
        int isOnFloor = cbc->body->IsOnFloor();
        if (!isOnFloor)
        {
            SwitchState<State_InAir>();
            return;
        }
    }

}

void State_OnGround::Draw()
{
}

void State_OnGround::DrawUI()
{
    debugWatchlist.Post("PlayerIsOnFloor: ", "%d", 1);
}

void State_OnGround::Exit()
{
    player.remove<OnGroundTag>();
}
