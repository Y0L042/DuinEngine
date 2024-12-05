#include "StateGameLoop.h"
#include "States.h"
#include "../Singletons.h"
#include "../EntityManager.h"
#include "../ECS.h"
#include "../PlayerStates/PlayerStates.h"
#include <flecs.h>

#include <iostream>

using namespace duin::ECSComponent;
using namespace duin::ECSTag;

duin::DebugWatchlist debugWatchlist;
duin::Physics3DServer pxServer;
duin::ECSManager ecsManager;
duin::GameStateMachine playerSM;

flecs::entity player;
flecs::entity cameraRoot;
flecs::entity fpsCamera;
flecs::entity debugCamera;

Model terrain;

physx::PxRigidDynamic *ball;

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
static void CookCollisions();

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
    duin::QueuePostPhysicsUpdateCallback(std::function<void(double)>([this](double delta) { pxServer.StepPhysics(delta); }));
    ecsManager.Initialize();

    flecs::entity testEntity;
    duin::JSONDocument testDef;
    duin::ReadJSONFile("./data/testDef.json", testDef);
    duin::JSONMember testECSData = testDef.GetMember("entity").GetMember("ecs_data");
    if (testECSData.IsValid()) {
        testEntity = ecsManager.CreateEntityFromJSON(testECSData);
    };

    float playerHeight = 1.75f;
    player = ecsManager.world.entity()
        .is_a(duin::ECSPrefab::CharacterBody3D)
        .set<Position3D, Local>({{ 0.0f, 5.0f, 0.0f }})
        .set<Mass>({ .value = 80.0f })
        .set<CanRunComponent>({ .speed = 10.0f })
        .set<CanSprintComponent>({ .speed = 17.5f })
        .set<CanJumpComponent>({ .impulse = 525.0f })
        .set<DebugCapsuleComponent>({ 1.75f, 0.35f, 8, 16, ::GREEN })
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
        .add<MouseInputVec2>()
        .add<CameraPitchComponent>()
        ;
    fpsCamera = ecsManager.world.entity()
        .is_a(duin::ECSPrefab::Camera3D)
        .child_of(cameraRoot)
        .set<Position3D, Local>({{ 0.0f, playerHeight, 0.0f }})
        .set<::Camera3D>({
                .target = { 0.0f, 0.0f, 0.0f },
                .up = { 0.0f, 1.0f, 0.0f },
                .fovy = 72.0f,
                .projection = ::CAMERA_PERSPECTIVE
            })
        // .set<VelocityBob>({ 10.0f, 5.0f })
        .set<DebugCapsuleComponent>({ 0.5f, 0.25f, 8, 16, ::RED })
        .add<duin::ECSTag::ActiveCamera>()
        ;
    duin::CharacterBody3DDesc desc = {
        .height = playerHeight,
        .radius = 0.5f,
        .slopeLimit = std::cosf(physx::PxPi / 4.0),
        .stepOffset = 0.5f,
        .contactOffset = 0.1f,
        .position = duin::Vector3(0.0f, playerHeight / 2.0f + 15, 0.0f),
        .upDirection = duin::Vector3(0.0f, 1.0f, 0.0f),
    };
    static duin::CharacterBody3D playerBody(pxServer);
    player.set<duin::ECSComponent::CharacterBody3DComponent >({ &playerBody });



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
        ;



    duin::StaticCollisionPlane ground(pxServer);
    ecsManager.ActivateCameraEntity(debugCamera);


    physx::PxMaterial *pxBallMaterial = pxServer.pxPhysics->createMaterial(0.2f, 0.1f, 0.6f);
    ball = physx::PxCreateDynamic(*pxServer.pxPhysics,
        physx::PxTransform(physx::PxVec3(0, 1, 2)),
        physx::PxSphereGeometry(1),
        *pxBallMaterial,
        10.0f);
    ball->setAngularDamping(0.5f);
    ball->setLinearVelocity(physx::PxVec3(0, -9.81, 1));
    pxServer.pxScene->addActor(*ball);



    SetFPSCamera(1);
    ecsManager.ActivateCameraEntity(fpsCamera);


    CookCollisions();

    playerSM.SwitchState<PlayerStateOnGround>();
}

void StateGameLoop::State_Exit()
{
}

void StateGameLoop::State_HandleInput()
{
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



    playerSM.ExecuteHandleInput();

    duin::Vector2 mouseInput(GetMouseDelta());


    if (fpsCameraEnabled) {
        player.set<MouseInputVec2>({ { mouseInput } });
        cameraRoot.set<MouseInputVec2>({ { mouseInput } });
    }
    else {
        debugCamera.set<MouseInputVec2>({ { mouseInput } });
    }

    int isOnFloor = player.get<CharacterBody3DComponent>()->characterBody->IsOnFloor();
    if (isOnFloor) {
        player.add<OnGroundTag>();
        player.remove<InAirTag>();
        debugWatchlist.Post("PlayerIsOnFloor: ", "%d", isOnFloor);
    }
    else {
        player.remove<OnGroundTag>();
        player.add<InAirTag>();
        debugWatchlist.Post("PlayerIsOnFloor: ", "%d", isOnFloor);
    }

    if (IsKeyPressed(KEY_SPACE) && isOnFloor) {
        player.add<JumpTag>();
    }
}

void StateGameLoop::State_Update(double delta)
{
    playerSM.ExecuteUpdate(delta);

    debugWatchlist.Post("FPS | Frametime", "%d | %.1f", GetFPS(), 1000 * GetFrameTime());

    const duin::Vector3 playerPosG = player.get<duin::ECSComponent::Position3D, duin::ECSTag::Global>()->value;
    debugCamera.set<DebugCameraTarget>({ { playerPosG } });

    
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
    ExecuteQueryResolveInputForces(ecsManager.world);
    ExecuteQueryResolveInputVelocities(ecsManager.world);

    // pxServer.StepPhysics(delta);

     const duin::Vector3 playerPosL = player.get<duin::ECSComponent::CharacterBody3DComponent>()->characterBody->GetFootPosition();
     debugWatchlist.Post("FootPos: ", "{ %.1f, %.1f, %.1f }", playerPosL.x, playerPosL.y, playerPosL.z);
    //
    // const duin::Vector3 playerPosG = player.get<duin::ECSComponent::Position3D, duin::ECSTag::Global>()->value;
    // debugWatchlist.Post("PlayerPosGlobal: ", "{ %.1f, %.1f, %.1f }", playerPosG.x, playerPosG.y, playerPosG.z);
    //
    // const duin::Vector3 playerVel = player.get<duin::ECSComponent::Velocity3D>()->value;
    // debugWatchlist.Post("PlayerVel: ", "{ %.1f, %.1f, %.1f }", playerVel.x, playerVel.y, playerVel.z);

    //const Camera3D* camera = ecsManager.world.get<Camera3D>();
    //debugWatchlist.Post("Camera: ", "{ %.1f, %.1f, %.1f }", camera->position.x, camera->position.y, camera->position.z);

}

void StateGameLoop::State_Draw()
{
    playerSM.ExecuteDraw();

    DrawGrid(1000, 10.0f);

    DrawModel(terrain, { 0.0f, -2.0f, 0.0f }, 2, BLACK);

    duin::Vector3 pxBodyPos = duin::Vector3(player.get<CharacterBody3DComponent>()->characterBody->GetPxController()->getFootPosition());
    DrawCapsuleWires(pxBodyPos.ToRaylib(),
        Vector3Add(pxBodyPos, { 0.0f, 1.75f, 0.0f }).ToRaylib(),
        0.3f,
        12, 16,
        BLUE
    );
    DrawCube({0.0f, 0.0f, 0.0f}, 2.0f, 2.0f, 2.0f, RED);
    DrawSphereWires(duin::Vector3(ball->getGlobalPose().p).ToRaylib(), 1.0f, 32, 32, RED);
    
}

void StateGameLoop::State_DrawUI()
{
    playerSM.ExecuteDrawUI();

    const duin::Vector3 playerVel = player.get<duin::ECSComponent::Velocity3D>()->value;
    // debugConsole.Log("Vel: { %.1f, %.1f, %.1f }", playerVel.x, playerVel.y, playerVel.z);
    debugWatchlist.Post("Vel:", "{ %.1f, %.1f, %.1f }", playerVel.x, playerVel.y, playerVel.z);
    debugWatchlist.Post("Speed:", "%.1f", duin::Vector3Length(playerVel));

    debugWatchlist.Draw("Watchlist");

    duin::DrawPhysicsFPS(15, 50);
    duin::DrawRenderFPS(15, 15);
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

static void CookCollisions()
{
    physx::PxRigidStatic *staticActor = pxServer.pxPhysics->createRigidStatic(physx::PxTransform(physx::PxVec3(0, 0, 0)));

    const physx::PxVec3 convexVerts[] = {
        physx::PxVec3(-1, -1, -1), // Bottom-left-back corner
        physx::PxVec3(1, -1, -1),  // Bottom-right-back corner
        physx::PxVec3(1, -1, 1),   // Bottom-right-front corner
        physx::PxVec3(-1, -1, 1),  // Bottom-left-front corner
        physx::PxVec3(-1, 1, -1),  // Top-left-back corner
        physx::PxVec3(1, 1, -1),   // Top-right-back corner
        physx::PxVec3(1, 1, 1),    // Top-right-front corner
        physx::PxVec3(-1, 1, 1)    // Top-left-front corner
    };
    physx::PxConvexMeshDesc convexDesc;
    convexDesc.points.count     = 8;
    convexDesc.points.stride    = sizeof(physx::PxVec3);
    convexDesc.points.data      = convexVerts;
    convexDesc.flags            = physx::PxConvexFlag::eCOMPUTE_CONVEX;
    physx::PxTolerancesScale scale;
    physx::PxCookingParams params(scale);

    physx::PxDefaultMemoryOutputStream buf;
    physx::PxConvexMeshCookingResult::Enum result;
    if(!PxCookConvexMesh(params, convexDesc, buf, &result))
        return;
    physx::PxDefaultMemoryInputData input(buf.getData(), buf.getSize());
    physx::PxConvexMesh* convexMesh = pxServer.pxPhysics->createConvexMesh(input);
    physx::PxShape* aConvexShape = physx::PxRigidActorExt::createExclusiveShape(
            *staticActor,
            physx::PxConvexMeshGeometry(convexMesh), 
            *pxServer.pxMaterial);

    pxServer.pxScene->addActor(*staticActor);
}

