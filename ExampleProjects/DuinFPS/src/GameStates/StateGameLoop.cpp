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

flecs::entity *test;

static void SetFPSCamera(int enable);
static flecs::entity ConstructPlayer();

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

}

void StateGameLoop::State_Update(double delta)
{
    Duin::SetActiveCamera3D(*(fpsCamera.get<Camera3D>()));

    if (fpsCameraEnabled && !IsCursorHidden()) {
        SetFPSCamera(1);
    }

    ecsManager.ExecuteQueryControlPlayerMovement(delta);
    // ecsManager.ExecuteQueryControlPlayerRotation(delta);

    ecsManager.ExecuteQueryUpdatePlayerYaw(delta);
    ecsManager.ExecuteQueryUpdateCameraPitch(delta);
}

void StateGameLoop::State_PhysicsUpdate(double delta)
{
    ecsManager.ExecuteQueryMovementInput();
    ecsManager.ExecuteQueryRotationInput();

    ecsManager.ExecuteQueryUpdatePosition3D(delta);
    ecsManager.ExecuteQueryHierarchicalUpdatePosition3D();

    ecsManager.ExecuteQueryUpdateRotation3D(delta);
    ecsManager.ExecuteQueryHierarchicalUpdateRotation3D();

    ecsManager.ExecuteQueryControlCamera(delta);
}

void StateGameLoop::State_Draw()
{
    DrawGrid(100, 1.0f);
}

void StateGameLoop::State_DrawUI()
{

    const Rotation3D *pRot = (fpsCamera).get<Rotation3D, Global>();
    if (pRot)
        // dbgConsole.LogEx(Duin::LogLevel::Info, "pRot{ %.1f, %.1f, %.1f }", pRot->value.x, pRot->value.y, pRot->value.z);
        debugWatchlist.Post("pRot", "{ %.1f, %.1f, %.1f }", pRot->value.x, pRot->value.y, pRot->value.z);

    const Camera3D *pCam = (fpsCamera).get<Camera3D>();
    if (pCam)
        // dbgConsole.LogEx(Duin::LogLevel::Info, "pCam{ %.1f, %.1f, %.1f }", pCam->position.x, pCam->position.y, pCam->position.z);
        debugWatchlist.Post("pCam", "{ %.1f, %.1f, %.1f }", pCam->position.x, pCam->position.y, pCam->position.z);

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
        ;

    fpsCamera = ecsManager.world.entity()
        .is_a(Node3D)
        .child_of(cameraRoot)
        .set<Camera3D>({
            .position = { 0.0f, 0.0f, 0.0f },
            .target = { 0.0f, 0.0f, 0.0f },
            .up = { 0.0f, 1.0f, 0.0f },
            .fovy = 60.0f,
            .projection = CAMERA_PERSPECTIVE
            })
        .add<CameraTag>()
        ;

    return entity;
}

