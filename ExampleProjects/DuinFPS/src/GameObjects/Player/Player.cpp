#include "Player.h"
#include "../../Singletons.h"
#include "../../ECS.h"
#include "States.h"

#include <Duin.h>

#include <Duin/Scene/SceneBuilder.h>
#include <Duin/IO/IOModule.h>

void Player::Ready()
{
}

void Player::Init()
{
    debugConsole.Log("Player: Ready");

    player = world->Lookup("Player");
    cameraRoot = world->Lookup("Player::CameraRoot");
    playerCamera = world->Lookup("Player::CameraRoot::PlayerCamera");

    blackboard.world = world;
    blackboard.player = player;
    blackboard.cameraRoot = cameraRoot;
    blackboard.playerCamera = playerCamera;

    playerStateMachine = CreateChildObject<PlayerStateMachine>(&blackboard);
    playerStateMachine->SwitchState<State_OnGround>();
    world->SetGlobalPosition(player, {1, 10, 1});
}

void Player::OnEvent(duin::Event e)
{
}

void Player::PhysicsUpdate(double delta)
{
    if (!(player.IsValid() && cameraRoot.IsValid()))
    {
        DN_WARN("Player {} CameraRoot {}", player.IsValid(), cameraRoot.IsValid());
        return;
    }

    duin::Vector2 mouseInput(duin::Input::GetMouseDelta());
    player.Set<MouseInputVec2>({{mouseInput}});
    cameraRoot.Set<MouseInputVec2>({{mouseInput}});

    const auto &ppos = world->GetGlobalPosition(player);
    const auto *cam = cameraRoot.TryGet<duin::Camera>();
    debugWatchlist.Post("Player Pos", "{ %.2f, %.2f, %.2f }", ppos.x, ppos.y, ppos.z);
    if (cam)
    {
        auto &lookAt = cam->target;
        debugWatchlist.Post("Player LookAt", "{ %.2f, %.2f, %.2f }", lookAt.x, lookAt.y, lookAt.z);
    }
}

void Player::DrawUI()
{
}
