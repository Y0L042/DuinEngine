#include "GameState_Gameplay.h"

#include "Singletons.h"
#include "GameObjects/Player/Player.h"
#include "ECS.h"

#include <Duin/ECS/ECSModule.h>
#include <Duin/Scene/SceneBuilder.h>
#include <Duin/IO/FileModule.h>
#include <Duin/Render/RenderModule.h>

void GameState_Gameplay::Enter()
{
    debugConsole.Log("GameState_Gameplay: Entering GameState_Gameplay");
    duin::Input::CaptureMouse(true);
    world = std::make_unique<duin::GameWorld>();
    world->Initialize();
    RegisterComponents(*world);

    duin::SceneBuilder sceneBuilder;
    auto pscn = sceneBuilder.DeserializeSceneFromFile("bin://data/world1.pscn");
    sceneBuilder.InstantiateScene(pscn, world.get());

    duin::Quaternion groundDir = duin::Vector3ToQuaternion(duin::Vector3::UP);
    duin::Vector3 groundPos = duin::Vector3::UP;
    static duin::StaticBody ground({groundPos, groundDir} /* Tx3D */, duin::PlaneGeometry(),
                                   duin::PhysicsMaterial(0.4f, 0.4f, 0.5f));
    playerObj = CreateChildObject<Player>(world.get());
}

void GameState_Gameplay::OnEvent(duin::Event e)
{
}

void GameState_Gameplay::Update(double delta)
{
}

namespace
{
    void UpdateCamera(duin::GameWorld &world)
    {
        ExecuteQueryUpdatePlayerYaw(world);
        ExecuteQueryUpdateCameraPitch(world);
        ExecuteQueryMoveDebugCamera(world);
    }

    void ComputeMovement(duin::GameWorld &world)
    {
        ExecuteQueryComputePlayerInputVelocity(world);
        ExecuteQueryIdle(world);
        ExecuteQueryRun(world);
        ExecuteQuerySprint(world);
    }

    void ResolvePhysics(duin::GameWorld &world)
    {
        ExecuteQueryGravity(world);
        ExecuteQueryOnGroundJump(world);
        ExecuteQueryResolveInputVelocities(world);
        ExecuteQueryResolveInputForces(world);
    }
} // namespace

void GameState_Gameplay::PhysicsUpdate(double delta)
{
    UpdateCamera(*world);
    ComputeMovement(*world);
    ResolvePhysics(*world);
}

void GameState_Gameplay::Draw()
{
    duin::DrawBox({0.0f, -20.0f, 0.0f}, duin::QuaternionIdentity(), {40.0f, 40.0f, 40.0f});
    duin::BeginDebugDraw();
    duin::DrawGrid();
    duin::EndDebugDraw();
}

void GameState_Gameplay::DrawUI()
{
}

void GameState_Gameplay::Exit()
{
}
