#include "State_OnGround.h"
#include "States.h"

#include "../../Singletons.h"
#include "../../ECS.h"

#include <Duin/Core/Utils/UtilsModule.h>
#include <Duin/ECS/ECSModule.h>

using namespace duin::ECSComponent;
using namespace duin::ECSTag;

std::shared_ptr<PlayerStateMachine> onGroundStateMachine;

void State_OnGround::Enter()
{
    debugConsole.Log("State_OnGround: Entering State_OnGround");

    duin::Entity& player = *GetBlackboard()->player;

    player.Add<OnGroundTag>();
    player.Remove<CanGravity>();
    onGroundStateMachine = CreateChildObject<PlayerStateMachine>(GetBlackboard());
    onGroundStateMachine->SwitchState<State_OnGround_Idle>();
}

void State_OnGround::OnEvent(duin::Event e)
{
    duin::Entity& player = *GetBlackboard()->player;

    if (duin::Input::IsKeyPressed(DN_KEY_SPACE))
    {
        player.Add<JumpTag>();
    }

}

void State_OnGround::Update(double delta)
{
}

void State_OnGround::PhysicsUpdate(double delta)
{
    duin::Entity& player = *GetBlackboard()->player;

    const CharacterBodyComponent *cbc = player.TryGet<CharacterBodyComponent>();
    if (cbc && cbc->body)
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
    duin::Entity& player = *GetBlackboard()->player;

    player.Remove<OnGroundTag>();
}
