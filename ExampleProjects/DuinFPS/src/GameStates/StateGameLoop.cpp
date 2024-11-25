#include "StateGameLoop.h"
#include "States.h"
#include "../Singletons.h"
#include "../EntityManager.h"
#include "../ECS.h"
#include <flecs.h>

using namespace duin::ECSComponent;
using namespace duin::ECSTag;

duin::DebugWatchlist debugWatchlist;
duin::Physics3DServer pxServer;
duin::ECSManager ecsManager;

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
    player = ecsManager.world.entity()
        .is_a(duin::ECSPrefab::CharacterBody3D)
        .add<InputVelocities>()
        .add<InputForces>()
        .add<PlayerMovementInputVec3>()
        .add<InputVelocityDirection>()
        .add<MouseInputVec2>()
        .add<CameraYawComponent>()
        .add<GravityComponent>()
        .set<Mass>({ .value = 80.0f })
        .set<CanRunComponent>({ .speed = 7.5f })
        .set<CanSprintComponent>({ .speed = 15.f })
        .set<CanJumpComponent>({ .impulse = 525.0f })
        .set<DebugCapsuleComponent>({ 1.75f, 0.35f, 8, 16, ::GREEN })
        .add<OnGroundTag>()
        .add<CanGravity>()
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
                .position = { 0.0f, 0.0f, 0.0f },
                .target = { 0.0f, 0.0f, 0.0f },
                .up = { 0.0f, 1.0f, 0.0f },
                .fovy = 72.0f,
                .projection = ::CAMERA_PERSPECTIVE
            })
        .add<duin::ECSTag::ActiveCamera>()
        ;
    duin::CharacterBody3DDesc desc = {
        .height = playerHeight,
        .radius = 0.5f,
        .slopeLimit = std::cosf(physx::PxPi / 4.0),
        .stepOffset = 0.5f,
        .contactOffset = 0.1f,
        .position = duin::Vector3(0.0f, playerHeight / 2.0f, 0.0f),
        .upDirection = duin::Vector3(0.0f, 1.0f, 0.0f),
    };
    static duin::CharacterBody3D playerBody(pxServer);
    player.set<duin::ECSComponent::CharacterBody3DComponent >({ &playerBody });



    debugCamera = ecsManager.world.entity()
        .is_a(duin::ECSPrefab::Camera3D)
        .set<Position3D, Local>({ { 0.0f, 25.0f, 25.0f } })
        .set<::Camera3D>({
                 .target = { 0.0f, 0.0f, 0.0f },
                 .up = { 0.0f, 1.0f, 0.0f },
                 .fovy = 72.0f,
                 .projection = ::CAMERA_PERSPECTIVE
            })
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

    terrain = LoadModel("assets\\.maps\\testmap.obj");


    ecsManager.ActivateCameraEntity(fpsCamera);
    SetFPSCamera(0);

    duin::QueuePostPhysicsUpdateCallback(std::function<void(double)>([this](double delta) { pxServer.StepPhysics(delta); }));
}

void StateGameLoop::State_Exit()
{
}

void StateGameLoop::State_HandleInput()
{
    duin::Vector2 input(GetInputVector2D(KEY_W, KEY_S, KEY_A, KEY_D));
    player.set<PlayerMovementInputVec3>({ { input.x, 0.0f, input.y } });
    if (!duin::Vector2Equals(input, duin::Vector2Zero())) {
        if (IsKeyDown(KEY_LEFT_SHIFT)) {
            player.add<SprintTag>();
        }
        else {
            player.add<RunTag>();
        }
    }

    duin::Vector2 mouseInput(GetMouseDelta());
    player.set<MouseInputVec2>({ { mouseInput } });
    cameraRoot.set<MouseInputVec2>({ { mouseInput } });

    if (IsKeyPressed(KEY_P)) {
        if (debugCamera.has<ActiveCamera>()) {
            ecsManager.ActivateCameraEntity(fpsCamera);
            SetFPSCamera(0);
        }
        else {
            ecsManager.ActivateCameraEntity(debugCamera);
            SetFPSCamera(1);
        }
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
    debugWatchlist.Post("FPS | Frametime", "%d | %.1f", GetFPS(), 1000 * GetFrameTime());

    const duin::Vector3 playerPosG = player.get<duin::ECSComponent::Position3D, duin::ECSTag::Global>()->value;
    debugCamera.set<DebugCameraTarget>({ { playerPosG } });

    
}

void StateGameLoop::State_PhysicsUpdate(double delta)
{    
    ExecuteQueryComputePlayerInputVelocity(ecsManager.world);
    ExecuteQueryUpdatePlayerYaw(ecsManager.world);
    ExecuteQueryUpdateCameraPitch(ecsManager.world);

    ExecuteQueryRun(ecsManager.world);
    ExecuteQuerySprint(ecsManager.world);
    ExecuteQueryJump(ecsManager.world);

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
    DrawGrid(1000, 10.0f);

    DrawModel(terrain, { 0.0f, -2.0f, 0.0f }, 2, BLACK);

    duin::Vector3 pxBodyPos = duin::Vector3(player.get<CharacterBody3DComponent>()->characterBody->GetPxController()->getFootPosition());
    DrawCapsuleWires(pxBodyPos.ToRaylib(),
        Vector3Add(pxBodyPos, { 0.0f, 1.75f, 0.0f }).ToRaylib(),
        0.3f,
        12, 16,
        BLUE
    );
    DrawSphereWires(duin::Vector3(ball->getGlobalPose().p).ToRaylib(), 1.0f, 32, 32, RED);
    
}

void StateGameLoop::State_DrawUI()
{
    const duin::Vector3 playerVel = player.get<duin::ECSComponent::Velocity3D>()->value;
    // debugConsole.Log("Vel: { %.1f, %.1f, %.1f }", playerVel.x, playerVel.y, playerVel.z);
    debugWatchlist.Post("Vel:", "{ %.1f, %.1f, %.1f }", playerVel.x, playerVel.y, playerVel.z);
    debugWatchlist.Post("Speed:", "%.1f", duin::Vector3Length(playerVel));

    debugWatchlist.Draw("Watchlist");
}

static void SetFPSCamera(int enable)
{
    fpsCameraEnabled = enable;
    if (!fpsCameraEnabled) {
        DisableCursor();
    } else {
        EnableCursor();
    }
}
