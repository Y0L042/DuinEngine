#include "StateGameLoop.h"
#include "States.h"
#include "../Singletons.h"
#include "../EntityManager.h"
#include "../Components.h"
#include "../ECS.h"


Duin::DebugWatchlist debugWatchlist;

std::string ENTITY_DEFS_PATH = "data/entity_defs.json";

int fpsCameraEnabled = 1;
EntityManager entityManager(ENTITY_DEFS_PATH);
ECSManager ecsManager;

/* Prefabs */
const char *node3DName = "Node3D";
flecs::entity Node3D;
const char *characterBody3DName = "CharacterBody3D";
flecs::entity CharacterBody3D;

flecs::entity player;
flecs::entity cameraRoot;
flecs::entity fpsCamera;

/* ----- PhysX Test ----- */
physx::PxDefaultAllocator defaultAllocatorCallback;
physx::PxDefaultErrorCallback defaultErrorCallback;
physx::PxDefaultCpuDispatcher *pxDispatcher = NULL;

physx::PxTolerancesScale pxToleranceScale;
physx::PxFoundation *pxFoundation = NULL;
physx::PxPhysics *pxPhysics = NULL;

physx::PxScene *pxScene = NULL;
physx::PxMaterial *pxMaterial = NULL;

physx::PxPvd *pxPvd = NULL;

physx::PxReal stackZ = 10.0f;

physx::PxRigidDynamic *pxBall = NULL;
/* ----- PhysX Test ----- */

static void SetFPSCamera(int enable);
static flecs::entity ConstructPlayer();

/* ----- PhysX Test ----- */
static physx::PxRigidDynamic* CreateDynamic(    const physx::PxTransform& t, 
                                                const physx::PxGeometry& geometry, 
                                                const physx::PxVec3& velocity = physx::PxVec3(0))
{
    physx::PxRigidDynamic* dynamic = physx::PxCreateDynamic(    *pxPhysics, 
                                                                t,
                                                                geometry, 
                                                                *pxMaterial, 
                                                                10.0f);
    dynamic->setAngularDamping(0.5f);
    dynamic->setLinearVelocity(velocity);
    pxScene->addActor(*dynamic);
    return dynamic;
}


static void InitPhysics(bool interactive)
{
	pxFoundation = PxCreateFoundation(  PX_PHYSICS_VERSION, 
                                        defaultAllocatorCallback, 
                                        defaultErrorCallback);

	pxPvd = physx::PxCreatePvd(*pxFoundation);
	physx::PxPvdTransport* transport = physx::PxDefaultPvdSocketTransportCreate("127.0.0.1", 5425, 10);
	pxPvd->connect(*transport, physx::PxPvdInstrumentationFlag::eALL);

	pxPhysics = PxCreatePhysics(PX_PHYSICS_VERSION, *pxFoundation, physx::PxTolerancesScale(), true, pxPvd);

	physx::PxSceneDesc sceneDesc(pxPhysics->getTolerancesScale());
	sceneDesc.gravity = physx::PxVec3(0.0f, -9.81f, 0.0f);
	pxDispatcher = physx::PxDefaultCpuDispatcherCreate(2);
	sceneDesc.cpuDispatcher	= pxDispatcher;
	sceneDesc.filterShader	= physx::PxDefaultSimulationFilterShader;
	pxScene = pxPhysics->createScene(sceneDesc);

	physx::PxPvdSceneClient* pvdClient = pxScene->getScenePvdClient();
	if(pvdClient)
	{
		pvdClient->setScenePvdFlag(physx::PxPvdSceneFlag::eTRANSMIT_CONSTRAINTS, true);
		pvdClient->setScenePvdFlag(physx::PxPvdSceneFlag::eTRANSMIT_CONTACTS, true);
		pvdClient->setScenePvdFlag(physx::PxPvdSceneFlag::eTRANSMIT_SCENEQUERIES, true);
	}
	pxMaterial = pxPhysics->createMaterial(0.5f, 0.5f, 0.6f);

	physx::PxRigidStatic* groundPlane = physx::PxCreatePlane(*pxPhysics, physx::PxPlane(0,1,0,0), *pxMaterial);
	pxScene->addActor(*groundPlane);

	if (!interactive)
		pxBall = CreateDynamic(physx::PxTransform(physx::PxVec3(0,40,100)), physx::PxSphereGeometry(10), physx::PxVec3(0,-50,-5));
}

static void StepPhysics(bool /*interactive*/)
{
	pxScene->simulate(1.0f/60.0f);
	pxScene->fetchResults(true);
}

static void CleanupPhysics(bool /*interactive*/)
{
	PX_RELEASE(pxScene);
	PX_RELEASE(pxDispatcher);
	PX_RELEASE(pxPhysics);
	if(pxPvd)
	{
		physx::PxPvdTransport* transport = pxPvd->getTransport();
		PX_RELEASE(pxPvd);
		PX_RELEASE(transport);
	}
	PX_RELEASE(pxFoundation);
	
	printf("SnippetHelloWorld done.\n");
}
/* ----- PhysX Test ----- */

StateGameLoop::StateGameLoop(Duin::GameStateMachine& owner)
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
    InitPhysics(false);

    ecsManager.RegisterComponents();
    ecsManager.CreateQueries();

    Node3D = ecsManager.world.prefab(node3DName)
        .add<Position3D, Global>()
        .add<Position3D, Local>()
        .add<Rotation3D, Global>()
        .add<Rotation3D, Local>()
        .add<Scale3D, Global>()
        .add<Scale3D, Local>();

    CharacterBody3D = ecsManager.world.prefab(characterBody3DName)
        .is_a(Node3D)
        .add<Velocity3D>()
        .add<AngularVelocity3D>();


    player = ConstructPlayer();

    /* ecsManager.world.entity()
        .add<Position3D>()
        .add<Velocity3D>(); */

    dbgConsole.LogEx(Duin::LogLevel::Info, "Entity: %ull", player);

    SetWindowFocused();
    SetFPSCamera(1);


}

void StateGameLoop::State_Exit()
{
    CleanupPhysics(false);
}

void StateGameLoop::State_HandleInput()
{
    int U, D, L, R = 0;
    float x, y = 0.0f;

    U = IsKeyDown(KEY_W);
    D = IsKeyDown(KEY_S);
    L = IsKeyDown(KEY_A);
    R = IsKeyDown(KEY_D);
    x = (float)(R - L);
    y = (float)(U - D);
    Vector2 input = Vector2Normalize(Vector2{ x, y });
    
    PlayerMovementInputVec2 mI = input;
    player.set<PlayerMovementInputVec2>(input);


    if (IsKeyPressed(KEY_P)) {
        SetFPSCamera(!fpsCameraEnabled);
    }

    Vector2 mouseDelta = GetMouseDelta();
    cameraRoot.set<MouseInputVec2>(mouseDelta);
    player.set<MouseInputVec2>(mouseDelta);
    debugWatchlist.Post("MouseInput", "{ %.1f, %.1f }", mouseDelta.x, mouseDelta.y);

    if (IsKeyPressed(KEY_SPACE)) {
        pxBall->setLinearVelocity({0, 20, 5});
    }
}

void StateGameLoop::State_Update(double delta)
{
    StepPhysics(true);
    Duin::SetActiveCamera3D(*(fpsCamera.get<Camera3D>()));

    if (fpsCameraEnabled && !IsCursorHidden()) {
        SetFPSCamera(1);
    }

    ecsManager.ExecuteQueryControlPlayerMovement(delta);

    ecsManager.ExecuteQueryUpdatePlayerYaw(delta);
    ecsManager.ExecuteQueryUpdateCameraPitch(delta);
}

void StateGameLoop::State_PhysicsUpdate(double delta)
{
    ecsManager.ExecuteQueryMovementInput();

    ecsManager.ExecuteQueryUpdatePosition3D(delta);
    ecsManager.ExecuteQueryHierarchicalUpdatePosition3D();

    ecsManager.ExecuteQueryUpdateRotation3D(delta);
    ecsManager.ExecuteQueryHierarchicalUpdateRotation3D();

    ecsManager.ExecuteQueryControlCamera(delta);
}

void StateGameLoop::State_Draw()
{
    physx::PxTransform globalPose = pxBall->getGlobalPose();
    physx::PxVec3 pos = globalPose.p;
    DrawSphereWires({pos.x, pos.y, pos.z}, 10, 8, 8, BLUE);

    DrawGrid(10000, 10.0f);
}

void StateGameLoop::State_DrawUI()
{

    const Rotation3D *playerRot = player.get<Rotation3D, Local>();
    if (playerRot)
        debugWatchlist.Post("PlayerRot", "{ %.1f, %.1f, %.1f %.1f }", 
                            playerRot->value.x, 
                            playerRot->value.y, 
                            playerRot->value.z, 
                            playerRot->value.w);

    const Rotation3D *pRot = (fpsCamera).get<Rotation3D, Global>();
    if (pRot)
        debugWatchlist.Post("pRot", "{ %.1f, %.1f, %.1f, %.1f }", 
                            pRot->value.x, 
                            pRot->value.y, 
                            pRot->value.z,
                            pRot->value.w);

    const Camera3D *pCam = (fpsCamera).get<Camera3D>();
    if (pCam)
        debugWatchlist.Post("pCam", "{ %.1f, %.1f, %.1f }", 
                            pCam->position.x, 
                            pCam->position.y, 
                            pCam->position.z);

    debugWatchlist.Draw("Watchlist");
}

static void SetFPSCamera(int enable)
{
    fpsCameraEnabled = enable;
    if (fpsCameraEnabled) {
        HideCursor();
        DisableCursor();
    } else {
        ShowCursor();
        EnableCursor();
    }
}

static flecs::entity ConstructPlayer()
{
    flecs::entity entity = ecsManager.world.entity()
        .is_a(CharacterBody3D)
        .set<Position3D, Local>({ 0, 5, 0 })
        .add<MovementInput3D>()
        .add<RotationInput3D>()
        .add<PlayerMovementInputVec2>()
        .add<MouseInputVec2>()
        .add<PlayerTag>()
        ;

    cameraRoot = ecsManager.world.entity()
        .is_a(Node3D)
        .child_of(entity)
        .add<MouseInputVec2>()
        .add<RotationInput3D>()
        .add<CameraTag>()
        ;

    fpsCamera = ecsManager.world.entity()
        .is_a(Node3D)
        .child_of(cameraRoot)
        .set<Camera3D>({
            .position = { 0.0f, 0.0f, 0.0f },
            .target = { 0.0f, 0.0f, 0.0f },
            .up = { 0.0f, 1.0f, 0.0f },
            .fovy = 90.0f,
            .projection = CAMERA_PERSPECTIVE
            })
        ;

    return entity;
}

