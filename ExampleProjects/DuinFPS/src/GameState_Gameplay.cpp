#include "GameState_Gameplay.h"

#include "Singletons.h"
#include "GameObjects/Player/Player.h"
#include "ECS.h"

#include <Duin.h>

duin::ECSManager ecs;
std::shared_ptr<Player> playerObj;

void GameState_Gameplay::Enter()
{
	debugConsole.Log("GameState_Gameplay: Entering GameState_Gameplay");
    duin::Input::CaptureMouse(true);
    ecs.Initialize();
    RegisterComponents(ecs.world);

	playerObj = CreateChild<Player>();

    duin::PhysicsMaterial groundMaterial(0.4f, 0.4f, 0.5f);
    duin::PlaneGeometry groundGeometry;
    duin::CollisionShape groundCollision(groundGeometry, groundMaterial);
    duin::Quaternion groundDir = duin::Vector3ToQuaternion({ 0.0f, 1.0f, 0.0f });
    static duin::StaticBody ground({ {0.0f, 0.0f, 0.0f}, groundDir }, groundCollision);

    //duin::PlaneGeometry plane;
    //duin::CollisionShape ground(plane, duin::PhysicsMaterial());
    //static std::shared_ptr<duin::StaticBody> groundPlane = duin::StaticBody::Create(duin::Transform3D{duin::Vector3(), duin::Quaternion::Y }, ground);
}

void GameState_Gameplay::OnEvent(duin::Event e)
{
}

void GameState_Gameplay::Update(double delta)
{
}

void GameState_Gameplay::PhysicsUpdate(double delta)
{
    ExecuteQueryUpdatePlayerYaw(ecs.world);
    ExecuteQueryUpdateCameraPitch(ecs.world);
    ExecuteQueryComputePlayerInputVelocity(ecs.world);
    ExecuteQueryGravity(ecs.world);
    ExecuteQueryDebugCameraTarget(ecs.world);
    ExecuteQueryVelocityBob(ecs.world);

    ExecuteQueryMoveDebugCamera(ecs.world);

    ExecuteQueryIdle(ecs.world);
    ExecuteQueryRun(ecs.world);
    ExecuteQuerySprint(ecs.world);
    ExecuteQueryOnGroundJump(ecs.world);

    ExecuteQueryResolveInputVelocities(ecs.world);
    ExecuteQueryResolveInputForces(ecs.world);
}

void GameState_Gameplay::Draw()
{
    //duin::DrawPlane({20.0f, 0.0f, 20.0f});
    duin::DrawGrid();
}

void GameState_Gameplay::DrawUI()
{
}

void GameState_Gameplay::Exit()
{
}
