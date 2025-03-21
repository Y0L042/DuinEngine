#include "StateGameLoop.h"
#include "States.h"
#include "../Singletons.h"
#include "../EntityManager.h"
#include "../ECS.h"
#include "../PlayerStates/PlayerStates.h"

#define RLIGHTS_IMPLEMENTATION
#include "../rlights.h"

#include <flecs.h>

#include <memory>
#include <iostream>
#include <omp.h>

#define GLSL_VERSION            330

using namespace duin::ECSComponent;
using namespace duin::ECSTag;

duin::DebugWatchlist debugWatchlist;
duin::ECSManager ecsManager;
duin::GameStateMachine playerSM;

flecs::entity player;
flecs::entity cameraRoot;
flecs::entity fpsCamera;
flecs::entity debugCamera;

Shader shader;

static const char *TERRAIN_PATH = "./assets/.maps/testMap.glb";
Model terrain;

int fpsCameraEnabled = 1;
static int test;

struct Pos { 
    float x, y; 
};

struct Test {
   const char* test = "Hello!\0";
};

InputVector2DKeys MOVEMENT_KEYS{ KEY_W, KEY_S, KEY_A, KEY_D };

static void SetFPSCamera(int enable);

StateGameLoop::StateGameLoop(duin::GameStateMachine& owner)
	: GameState(owner)
{
    Enter = [this]() { State_Enter(); };
    Exit = [this]() { State_Exit(); };
    HandleInput = [this]() { State_HandleInput(); };
    Update = [this](double delta) { State_Update(delta); };
    PhysicsUpdate = [this](double delta) { State_PhysicsUpdate(delta); };
    Draw = [this]() { State_Draw(); };
    DrawUI = [this]() { State_DrawUI(); };
}

StateGameLoop::~StateGameLoop()
{
}

void StateGameLoop::State_Enter()
{
    ecsManager.Initialize();

    float playerHeight = 1.75f;
    duin::Vector3 playerPosition(0.0f, 0.01f, 5.0f);
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
    player = ecsManager.world.entity()
        .is_a(duin::ECSPrefab::PhysicsCharacterBody)
        .set<Position3D, Local>({ playerPosition })
        .set<Mass>({ .value = 80.0f })
        .set<CanRunComponent>({ .speed = 10.0f })
        .set<CanSprintComponent>({ .speed = 17.5f })
        .set<CanJumpComponent>({ .impulse = 625.0f })
        .set<CharacterBodyComponent >({ playerBody })
        .add<InputVelocities>()
        .add<InputForces>()
        .add<PlayerMovementInputVec3>()
        .add<InputVelocityDirection>()
        .add<MouseInputVec2>()
        .add<CameraYawComponent>()
        .add<GravityComponent>()
        .add<OnGroundTag>()
        ;
    cameraRoot = ecsManager.world.entity()
        .is_a(duin::ECSPrefab::Node3D)
        .child_of(player)
        .set<Position3D, Local>({{ 0.0f, playerHeight, 0.0f }})
        .add<MouseInputVec2>()
        .add<CameraPitchComponent>()
        ;
    fpsCamera = ecsManager.world.entity()
        .is_a(duin::ECSPrefab::Camera3D)
        .child_of(cameraRoot)
        .set<::Camera3D>({
                .target = { 0.0f, 0.0f, 0.0f },
                .up = { 0.0f, 1.0f, 0.0f },
                .fovy = 72.0f,
                .projection = ::CAMERA_PERSPECTIVE
            })
        .set<VelocityBob>({ 10.0f, 1.0f })
        .add<duin::ECSTag::ActiveCamera>()
        ;



    debugCamera = ecsManager.world.entity()
        .is_a(duin::ECSPrefab::Camera3D)
        .set<Position3D, Local>({ { 0.0f, 5.0f, 15.0f } })
        .set<::Camera3D>({
                 .target = { 0.0f, 0.0f, 0.0f },
                 .up = { 0.0f, 1.0f, 0.0f },
                 .fovy = 72.0f,
                 .projection = ::CAMERA_PERSPECTIVE
            })
        .add<MouseInputVec2>()
        .add<CameraPitchComponent>()
        .add<CameraYawComponent>()
        .add<DebugCameraTag>()
        ;


    duin::PhysicsMaterial groundMaterial(0.4f, 0.4f, 0.5f);
    duin::PlaneGeometry groundGeometry;
    duin::CollisionShape groundCollision(groundGeometry, groundMaterial);
    duin::Quaternion groundDir = duin::Vector3ToQuaternion({0.0f, 1.0f, 0.0f}) ;
    static duin::StaticBody ground({ {0.0f, 0.0f, 0.0f}, groundDir }, groundCollision);

    duin::Vector3 boxPos(3.0f, 3.0f, 3.0f);
    duin::Vector3 boxSize(2.0f, 2.0f, 2.0f);
    duin::PhysicsMaterial material(0.4f, 0.4f, 0.5f);
    duin::BoxGeometry boxGeometry(boxSize);
    duin::CollisionShape boxCollision(boxGeometry, material);
    duin::Quaternion boxDir = duin::Vector3ToQuaternion({0.0f, 1.0f, 0.0f}) ;
    duin::Transform3D boxTX(boxPos, boxDir);
    ecs_entity_t bouncyCube = ecsManager.world.entity()
        .is_a(duin::ECSPrefab::PhysicsDynamicBody)
        .set<Position3D, Local>({boxPos})
        .set<DynamicBodyComponent>({ duin::DynamicBody::Create(boxTX, boxCollision) })
        ;
    ecsManager.world.entity()
        .is_a(duin::ECSPrefab::Cube)
        .child_of(bouncyCube)
        .set<CubeComponent>({boxSize.x, boxSize.y, boxSize.z, GRAY})
        .set<Position3D, Local>({boxPos})
        ;

    ecsManager.ActivateCameraEntity(debugCamera);

    SetFPSCamera(1);
    ecsManager.ActivateCameraEntity(fpsCamera);

    playerSM.SwitchState<PlayerStateOnGround>();
}

void StateGameLoop::State_Exit()
{
    
}

void StateGameLoop::State_HandleInput()
{
    /* --- DEBUG --- */
    if (fpsCameraEnabled && !IsCursorHidden()) {
        SetFPSCamera(1);
    }

    if (IsKeyPressed(KEY_P)) {
        // If debugCamera is active, switch to fpsCamera,
        // else switch to debugCamera
        if (debugCamera.has<ActiveCamera>()) {
            ecsManager.ActivateCameraEntity(fpsCamera);
            SetFPSCamera(1);
        }
        else {
            ecsManager.ActivateCameraEntity(debugCamera);
            SetFPSCamera(0);
        }
    }

    // If debugCamera enabled
    if (!fpsCameraEnabled && IsKeyPressed(KEY_LEFT_CONTROL)) {
        int x = IsKeyPressed(KEY_D) - IsKeyPressed(KEY_A);
        int y = IsKeyPressed(KEY_Q) - IsKeyPressed(KEY_E);
        int z = IsKeyPressed(KEY_W) - IsKeyPressed(KEY_S);
        debugCamera.set<Movement3DInput>({ (float)x, (float)y, (float)z });
    }

    static int debugShapesEnabled = 0;
    if (IsKeyPressed(KEY_F3)) {
        if (debugShapesEnabled) {
            player.remove<DebugCapsuleComponent>();
        } else {
            player.set<DebugCapsuleComponent>({ 1.75f, 0.35f, 8, 16, ::GREEN });
        }
        debugShapesEnabled = !debugShapesEnabled;
    }
    /* --- DEBUG --- */


    playerSM.ExecuteHandleInput();
    duin::Vector2 mouseInput(GetMouseDelta());
    if (fpsCameraEnabled) {
        player.set<MouseInputVec2>({ { mouseInput } });
        cameraRoot.set<MouseInputVec2>({ { mouseInput } });
    }
    else {
        debugCamera.set<MouseInputVec2>({ { mouseInput } });
    }

    // int isOnFloor = 0;
    // const CharacterBodyComponent *cbc = player.get<CharacterBodyComponent>(); 
    // if (cbc) {
    //     isOnFloor = cbc->characterBody->IsOnFloor();
    //     if (isOnFloor) {
    //         player.add<OnGroundTag>();
    //         player.remove<InAirTag>();
    //         debugWatchlist.Post("PlayerIsOnFloor: ", "%d", isOnFloor);
    //     }
    //     else {
    //         player.remove<OnGroundTag>();
    //         player.add<InAirTag>();
    //         debugWatchlist.Post("PlayerIsOnFloor: ", "%d", isOnFloor);
    //     }
    // }

    // if (IsKeyPressed(KEY_SPACE) && isOnFloor) {
    //     player.add<JumpTag>();
    // }
}

void StateGameLoop::State_Update(double delta)
{
    playerSM.ExecuteUpdate(delta);

    debugWatchlist.Post("FPS | Frametime", "%d | %.1f", GetFPS(), 1000 * GetFrameTime());

    //const duin::Vector3 playerPosG = player.get<duin::ECSComponent::Position3D, duin::ECSTag::Global>()->value;
    //debugCamera.set<DebugCameraTarget>({ { playerPosG } });

    
}

void StateGameLoop::State_PhysicsUpdate(double delta)
{    
    playerSM.ExecutePhysicsUpdate(delta);

    ExecuteQueryComputePlayerInputVelocity(ecsManager.world);
    ExecuteQueryUpdatePlayerYaw(ecsManager.world);
    ExecuteQueryUpdateCameraPitch(ecsManager.world);
    ExecuteQueryVelocityBob(ecsManager.world);

    ExecuteQueryIdle(ecsManager.world);
    ExecuteQueryRun(ecsManager.world);
    ExecuteQuerySprint(ecsManager.world);
    ExecuteQueryOnGroundJump(ecsManager.world);

    ExecuteQueryGravity(ecsManager.world);
    ExecuteQueryDebugCameraTarget(ecsManager.world);
    ExecuteQueryMoveDebugCamera(ecsManager.world);
    ExecuteQueryResolveInputForces(ecsManager.world);
    ExecuteQueryResolveInputVelocities(ecsManager.world);

    duin::PhysicsServer::Get().StepPhysics(delta);

    // const duin::Vector3 playerPosL = player.get<duin::ECSComponent::CharacterBody3DComponent>()->characterBody->GetFootPosition();
    // debugWatchlist.Post("FootPos: ", "{ %.1f, %.1f, %.1f }", playerPosL.x, playerPosL.y, playerPosL.z);

    //const duin::Vector3 playerPosG = player.get<duin::ECSComponent::Position3D, duin::ECSTag::Global>()->value;
    //debugWatchlist.Post("PlayerPosition:", " { %.1f, %.1f, %.1f }", playerPosG.x, playerPosG.y, playerPosG.z);
    // debugConsole.LogEx(duin::LogLevel::Info, "PlayerPosition: { %.1f, %.1f, %.1f }", playerPosG.x, playerPosG.y, playerPosG.z);

    // debugWatchlist.Post("PlayerPosGlobal: ", "{ %.1f, %.1f, %.1f }", playerPosG.x, playerPosG.y, playerPosG.z);
    // const duin::Vector3 playerVel = player.get<duin::ECSComponent::Velocity3D>()->value;
    // debugWatchlist.Post("PlayerVel: ", "{ %.1f, %.1f, %.1f }", playerVel.x, playerVel.y, playerVel.z);

    //const Camera3D* camera = ecsManager.world.get<Camera3D>();
    //debugWatchlist.Post("Camera: ", "{ %.1f, %.1f, %.1f }", camera->position.x, camera->position.y, camera->position.z);

}

void StateGameLoop::State_Draw()
{
    playerSM.ExecuteDraw();

    ::DrawCubeWires({0.0f, 0.0f, 0.0f}, 1.0f, 1.0f, 1.0f, RED);
    // DrawCube({0.0f, 0.0f, 0.0f}, 2.9f, 2.9f, 2.9f, BLUE);
    // BeginShaderMode(shader);
    //     DrawCube({0.0f, 0.0f, 0.0f}, 3.0f, 3.0f, 3.0f, WHITE);
    //     DrawModel(terrain, { 0.0f, 0.0f, 0.0f }, 1, WHITE);
    // EndShaderMode();


    DrawGrid(1000, 10.0f);
}

void StateGameLoop::State_DrawUI()
{
    playerSM.ExecuteDrawUI();

    //const duin::Vector3 playerVel = player.get<duin::ECSComponent::Velocity3D>()->value;
    //// debugConsole.Log("Vel: { %.1f, %.1f, %.1f }", playerVel.x, playerVel.y, playerVel.z);
    //debugWatchlist.Post("Vel:", "{ %.1f, %.1f, %.1f }", playerVel.x, playerVel.y, playerVel.z);
    //duin::Vector3 hSpeed(playerVel.x, 0.0f, playerVel.z);
    //debugWatchlist.Post("Speed:", "Total: %.1f\t H: %.1f\t V: %.1f", 
    //        duin::Vector3LengthF(playerVel),
    //        duin::Vector3LengthF(hSpeed),
    //        playerVel.y);

    debugWatchlist.Draw("Watchlist");

    duin::DrawRenderFPS(15, 15);
    duin::DrawPhysicsFPS(15, 50);
}

static void SetFPSCamera(int enable)
{
    fpsCameraEnabled = enable;
    if (fpsCameraEnabled) {
        DisableCursor();
    } else {
        DisableCursor();
    }
}
