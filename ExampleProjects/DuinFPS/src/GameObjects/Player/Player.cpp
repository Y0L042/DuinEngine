#include "Player.h"
#include "../../Singletons.h"
#include "../../ECS.h"
#include "States.h"

#include <Duin.h>

#include <Duin/ECS/SceneBuilder.h>
#include <Duin/IO/IOModule.h>

flecs::entity player;
flecs::entity cameraRoot;
flecs::entity playerCamera;

flecs::ref<duin::Camera> cameraRef;
duin::StateMachine playerStateMachine;

void Player::Ready()
{
	debugConsole.Log("Player: Ready");

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
    player = ecs.world.entity()
        .is_a(duin::ECSPrefab::PhysicsCharacterBody)
        .set<duin::ECSComponent::Transform3D>({ playerPosition })
        .set<Mass>({ .value = 80.0f })
        .set<CanRunComponent>({ .speed = 10.0f })
        .set<CanSprintComponent>({ .speed = 17.5f })
        .set<CanJumpComponent>({ .impulse = 625.0f })
        .set<duin::ECSComponent::CharacterBodyComponent >({ playerBody })
        .add<InputVelocities>()
        .add<InputForces>()
        .add<PlayerMovementInputVec3>()
        .add<InputVelocityDirection>()
        .add<MouseInputVec2>()
        .add<CameraYawComponent>()
        .add<GravityComponent>()
        .add<CanGravity>()
        .add<OnGroundTag>()
        ;
    cameraRoot = ecs.world.entity()
        .is_a(duin::ECSPrefab::Node3D)
        .child_of(player)
        .set<duin::ECSComponent::Transform3D>({ { 0.0f, playerHeight, 0.0f } })
        .add<MouseInputVec2>()
        .add<CameraPitchComponent>()
        ;
    playerCamera = ecs.world.entity()
        .is_a(duin::ECSPrefab::Camera3D)
        .child_of(cameraRoot)
        .set<duin::ECSComponent::Transform3D>({ { 0.0f, 0.0f, 0.0f } })
        .set<duin::Camera>(duin::Camera{
		       duin::UUID(),
               { 0.0f, 0.0f, 0.0f },
               { 0.0f, 0.0f, 0.0f },
               { 0.0f, 1.0f, 0.0f },
               72.0f
           })
        .set<VelocityBob>({ 10.0f, 1.0f })
        .add<duin::ECSTag::ActiveCamera>()
        ;

	//cameraRef = flecs::ref<duin::Camera>(ecs.world, playerCamera);
	//duin::Camera* cam = cameraRef.get();

    std::string scene;
    duin::SceneBuilder::WriteScene(scene, ecs);
    //DN_INFO("Scene:\n{}", scene);

    std::string plrjson;
    duin::SceneBuilder::WriteEntity(plrjson, player);
    //DN_INFO("Plr:\n{}", plrjson);

    std::string plrrecjson;
    duin::SceneBuilder::WriteEntity(plrrecjson, player, true);
    //DN_INFO("Plr Recursive:\n{}", plrrecjson);

    duin::FileUtils::WriteStringIntoFile("./recursive_entity.json", plrrecjson);
    duin::FileUtils::WriteStringIntoFile("./nonrecursive_entity.json", plrjson);
    duin::FileUtils::WriteStringIntoFile("./scene.json", scene);
    

    playerStateMachine.SwitchState<State_OnGround>();
}
void Player::OnEvent(duin::Event e)
{
    playerStateMachine.ExecuteOnEvent(e);
}

void Player::PhysicsUpdate(double delta)
{
    duin::Vector2 mouseInput(duin::Input::GetMouseDelta());
    player.set<MouseInputVec2>({ { mouseInput } });
    cameraRoot.set<MouseInputVec2>({ { mouseInput } });

    playerStateMachine.ExecutePhysicsUpdate(delta);
}

void Player::DrawUI()
{
    playerStateMachine.ExecuteDrawUI();
}

