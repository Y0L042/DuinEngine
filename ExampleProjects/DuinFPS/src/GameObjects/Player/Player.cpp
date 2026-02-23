#include "Player.h"
#include "../../Singletons.h"
#include "../../ECS.h"
#include "States.h"

#include <Duin.h>

#include <Duin/Scene/SceneBuilder.h>
#include <Duin/IO/IOModule.h>

#define WRITE_ENT 1

flecs::ref<duin::Camera> cameraRef;

void Player::Ready()
{
}

void Player::Init()
{
    debugConsole.Log("Player: Ready");

#if WRITE_ENT
    float playerHeight = 1.75f;
    duin::Vector3 playerPosition(0.0f, 50.0f, 5.0f);
    duin::CharacterBodyDesc playerDesc = {
        .height = playerHeight,
        .radius = 0.3f,
        .slopeLimit = std::cosf(physx::PxPi / 4.0),
        .stepOffset = 0.5f,
        .contactOffset = 0.1f,
        .position = duin::Vector3(playerPosition),
        .upDirection = duin::Vector3(0.0f, 1.0f, 0.0f),
    };
    duin::PhysicsMaterial playerMaterial(0.5f, 0.5f, 0.5f);
    std::shared_ptr<duin::CharacterBody> playerBody = duin::CharacterBody::Create(playerDesc);
#endif

    duin::SceneBuilder sceneBuilder(world.get());
    duin::Entity root = world->CreateEntity();
#if !WRITE_ENT
    duin::JSONValue scnJSON = duin::JSONValue::ParseFromFile(
        "C:\\Projects\\CPP_Projects\\Duin\\ExampleProjects\\Sandbox\\data\\playerScene.json");
    duin::PackedScene pscn = duin::PackedScene::Deserialize(scnJSON);
    sceneBuilder.InstantiateScene(pscn, world.get());

    player = world->Lookup("Player"); //.Set<duin::ECSComponent::CharacterBodyComponent>({playerBody});
    cameraRoot = world->Lookup("Player::CameraRoot");
    playerCamera = world->Lookup("Player::CameraRoot::PlayerCamera");
#endif

#if WRITE_ENT
    player = world->CreateEntity("Player")
                 .ChildOf(root)
                 .IsA(duin::ECSPrefab::PhysicsCharacterBody)
                 .Set<duin::ECSComponent::Transform3D>({playerPosition})
                 .Set<Mass>({.value = 80.0f})
                 .Set<CanRunComponent>({.speed = 10.0f})
                 .Set<CanSprintComponent>({.speed = 17.5f})
                 .Set<CanJumpComponent>({.impulse = 625.0f})
                 .Set<duin::ECSComponent::CharacterBodyComponent>({playerBody})
                 .Add<InputVelocities>()
                 .Add<InputForces>()
                 .Add<PlayerMovementInputVec3>()
                 .Add<InputVelocityDirection>()
                 .Add<MouseInputVec2>()
                 .Add<CameraYawComponent>()
                 .Add<GravityComponent>()
                 .Add<CanGravity>()
                 .Add<OnGroundTag>();
    cameraRoot = world->CreateEntity("CameraRoot")
                     .IsA(duin::ECSPrefab::Node3D)
                     .ChildOf(player)
                     .Set<duin::ECSComponent::Transform3D>({{0.0f, playerHeight, 0.0f}})
                     .Add<MouseInputVec2>()
                     .Add<CameraPitchComponent>();
    playerCamera = world->CreateEntity("PlayerCamera")
                       .IsA(duin::ECSPrefab::Camera3D)
                       .ChildOf(cameraRoot)
                       .Set<duin::ECSComponent::Transform3D>({{0.0f, 0.0f, 0.0f}})
                       .Set<duin::Camera>(duin::Camera{
                           duin::UUID(), {0.0f, 0.0f, 0.0f}, {0.0f, 0.0f, 0.0f}, {0.0f, 1.0f, 0.0f}, 72.0f})
                       .Set<VelocityBob>({10.0f, 1.0f})
                       .Add<duin::ECSTag::ActiveCamera>();
#endif

#if 1
    duin::PackedScene pscn_ = sceneBuilder.PackScene({player});
    duin::JSONValue worldJSON = duin::PackedScene::Serialize(pscn_);
    DN_INFO("{}", worldJSON.Write());
#endif

    // cameraRef = flecs::ref<duin::Camera>(world->GetFlecsWorld(), playerCamera.GetFlecsEntity());
    // duin::Camera* cam = cameraRef.get();

    blackboard.player = &player;
    blackboard.cameraRoot = &cameraRoot;
    blackboard.playerCamera = &playerCamera;

    playerStateMachine = CreateChildObject<PlayerStateMachine>(&blackboard);
    playerStateMachine->SwitchState<State_OnGround>();
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
}

void Player::DrawUI()
{
}
