#include "GameState_Gameplay.h"

#include "Singletons.h"
#include "GameObjects/Player/Player.h"
#include "GameObjects/BeachBall/BeachBall.h"
#include "ECS.h"

#include <Duin.h>
#include <Duin/ECS/ECSModule.h>
#include <Duin/Scene/SceneBuilder.h>

std::unique_ptr<duin::GameWorld> world;
std::shared_ptr<Player> playerObj;

void GameState_Gameplay::Enter()
{
    debugConsole.Log("GameState_Gameplay: Entering GameState_Gameplay");
    duin::Input::CaptureMouse(true);
    world = std::make_unique<duin::GameWorld>();
    world->Initialize();
    RegisterComponents(*world);

    playerObj = CreateChildObject<Player>();

    duin::Quaternion groundDir = duin::Vector3ToQuaternion(duin::Vector3::UP);
    duin::Vector3 groundPos = duin::Vector3::UP;

    static duin::StaticBody ground({groundPos, groundDir} /* Tx3D */, duin::PlaneGeometry(),
                                   duin::PhysicsMaterial(0.4f, 0.4f, 0.5f));

    static std::shared_ptr<BeachBall> ball = CreateChildObject<BeachBall>();

}

void GameState_Gameplay::OnEvent(duin::Event e)
{
}

void GameState_Gameplay::Update(double delta)
{
}

void GameState_Gameplay::PhysicsUpdate(double delta)
{
    ExecuteQueryUpdatePlayerYaw(*world);
    ExecuteQueryUpdateCameraPitch(*world);
    ExecuteQueryComputePlayerInputVelocity(*world);
    ExecuteQueryGravity(*world);
    ExecuteQueryDebugCameraTarget(*world);
    ExecuteQueryVelocityBob(*world);

    ExecuteQueryMoveDebugCamera(*world);

    ExecuteQueryIdle(*world);
    ExecuteQueryRun(*world);
    ExecuteQuerySprint(*world);
    ExecuteQueryOnGroundJump(*world);

    ExecuteQueryResolveInputVelocities(*world);
    ExecuteQueryResolveInputForces(*world);
}

void GameState_Gameplay::Draw()
{
    // duin::DrawPlane({20.0f, 0.0f, 20.0f});
    // duin::DrawBox({ 0.0f, -20.0f, 0.0f }, duin::QuaternionIdentity(), {40.0f, 40.0f, 40.0f});
    duin::DrawGrid();
}

void GameState_Gameplay::DrawUI()
{
}

void GameState_Gameplay::Exit()
{
}
