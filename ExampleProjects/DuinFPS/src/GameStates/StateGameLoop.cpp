#include "StateGameLoop.h"
#include "States.h"
#include "../Singletons.h"
#include "../EntityManager.h"
#include "../ECS.h"
#include <flecs.h>

using namespace duin::ECSComponent;
using namespace duin::ECSTag;

duin::DebugWatchlist debugWatchlist;
duin::Physics3DServer pxManager;
duin::ECSManager ecsManager;

flecs::entity player;
flecs::entity cameraRoot;
flecs::entity fpsCamera;
flecs::entity debugCamera;

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
        .add<PlayerMovementInputVec3>()
        .add<MouseInputVec2>()
        .add<CameraYawComponent>()
        .add<GravityComponent>()
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
    ecsManager.world.entity()
        .is_a(duin::ECSPrefab::DebugCapsule)
        .child_of(player)
        ;
    duin::CharacterBody3DDesc desc = {
        .height = playerHeight,
        .radius = 0.5f,
        .slopeLimit = std::cosf(physx::PxPi / 4.0),
        .stepOffset = 0.5f,
        .contactOffset = 0.1f,
        .position = duin::Vector3(0.0f, playerHeight / 2.0f + 0.0001f, 0.0f),
        .upDirection = duin::Vector3(0.0f, 1.0f, 0.0f),
        .material = pxManager.pxPhysics->createMaterial(0.5f, 0.5f, 0.5f),
        .reportCallback = nullptr,
        .behaviourCallback = nullptr
    };
    static duin::CharacterBody3D playerBody(pxManager, desc);
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



    duin::StaticCollisionPlane ground(pxManager);
    ecsManager.ActivateCameraEntity(debugCamera);



    ball = physx::PxCreateDynamic(*pxManager.pxPhysics,
        physx::PxTransform(physx::PxVec3(0, 1, 0)),
        physx::PxSphereGeometry(1),
        *pxManager.pxMaterial,
        10.0f);
    ball->setAngularDamping(0.5f);
    ball->setLinearVelocity(physx::PxVec3(0, -9.81, 0));
    pxManager.pxScene->addActor(*ball);
}

void StateGameLoop::State_Exit()
{
}

void StateGameLoop::State_HandleInput()
{
    duin::Vector2 input(GetInputVector2D(KEY_W, KEY_S, KEY_A, KEY_D));
    player.set<PlayerMovementInputVec3>({ { input.x, 0.0f, input.y } });
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

    ExecuteQueryGravity(ecsManager.world);
    ExecuteQueryGravityDebugCameraTarget(ecsManager.world);

    const duin::Vector3 playerPosL = player.get<duin::ECSComponent::Position3D, duin::ECSTag::Local>()->value;
    debugWatchlist.Post("PlayerPosLocal: ", "{ %.1f, %.1f, %.1f }", playerPosL.x, playerPosL.y, playerPosL.z);

    const duin::Vector3 playerPosG = player.get<duin::ECSComponent::Position3D, duin::ECSTag::Global>()->value;
    debugWatchlist.Post("PlayerPosGlobal: ", "{ %.1f, %.1f, %.1f }", playerPosG.x, playerPosG.y, playerPosG.z);

    const duin::Vector3 playerVel = player.get<duin::ECSComponent::Velocity3D>()->value;
    debugWatchlist.Post("PlayerVel: ", "{ %.1f, %.1f, %.1f }", playerVel.x, playerVel.y, playerVel.z);

    const Camera3D* camera = ecsManager.world.get<Camera3D>();
    debugWatchlist.Post("Camera: ", "{ %.1f, %.1f, %.1f }", camera->position.x, camera->position.y, camera->position.z);

    pxManager.StepPhysics(delta);
}

void StateGameLoop::State_Draw()
{
    DrawGrid(1000, 10.0f);

    duin::Vector3 pxBodyPos = duin::Vector3(player.get<CharacterBody3DComponent>()->characterBody->controller->getFootPosition());
    DrawCapsuleWires(pxBodyPos.ToRaylib(),
        Vector3Add(pxBodyPos, { 0.0f, 1.75f, 0.0f }).ToRaylib(),
        1.0f,
        12, 16,
        BLUE
    );
    DrawSphereWires(duin::Vector3(ball->getGlobalPose().p).ToRaylib(), 1.0f, 32, 32, RED);
    
}

void StateGameLoop::State_DrawUI()
{
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
