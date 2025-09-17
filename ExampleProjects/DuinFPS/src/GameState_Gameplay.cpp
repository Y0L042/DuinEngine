#include "GameState_Gameplay.h"

#include "Singletons.h"
#include "GameObjects/Player/Player.h"
#include "GameObjects/BeachBall/BeachBall.h"
#include "ECS.h"

#include <Duin.h>
#include <Duin/ECS/ECSModule.h>

duin::ECSManager ecs;
std::shared_ptr<Player> playerObj;

duin::World world;

void GameState_Gameplay::Enter()
{
	debugConsole.Log("GameState_Gameplay: Entering GameState_Gameplay");
    duin::Input::CaptureMouse(true);
    ecs.Initialize();
    RegisterComponents(ecs.world);

	playerObj = CreateChild<Player>();

    duin::Quaternion groundDir = duin::Vector3ToQuaternion(duin::Vector3::UP);
    duin::Vector3 groundPos = duin::Vector3::UP;

    static duin::StaticBody ground({groundPos, groundDir} /* Tx3D */,  
                                   duin::PlaneGeometry(), 
                                   duin::PhysicsMaterial(0.4f, 0.4f, 0.5f));

    static std::shared_ptr<BeachBall> ball = CreateChild<BeachBall>();

	DN_INFO("Writing scenetree to Demo_Scenetree.ecst ...");
    std::string scene = "";
    duin::SceneBuilder::WriteScene(scene, ecs);
    duin::FileUtils::WriteStringIntoFile("./Demo_Scenetree.ecst", scene);
	DN_INFO("Wrote scenetree to Demo_Scenetree.ecst !");

	static duin::Entity testEntity = world.CreateEntity("TestEntity");
	testEntity
        .Set<duin::ECSComponent::Position3D>(duin::Vector3{0,0,0})
        .Add<duin::ECSComponent::DebugCubeComponent>();
	DN_INFO("TestEntity ID: {}", testEntity.GetID());
	DN_INFO("TestEntity component: {}", testEntity.Has<duin::ECSComponent::Position3D>());
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
	//duin::DrawBox({ 0.0f, -20.0f, 0.0f }, duin::QuaternionIdentity(), {40.0f, 40.0f, 40.0f});
    duin::DrawGrid();
}

void GameState_Gameplay::DrawUI()
{
}

void GameState_Gameplay::Exit()
{
}
