#include "EditorWorld.h"
#include <Duin/Render/RenderModule.h>

EditorWorld::EditorWorld()
{
}

EditorWorld::~EditorWorld()
{
}

void EditorWorld::ResetQueries()
{
    queryEntityPositionGizmos.Destruct();
    duin::GameWorld::ResetQueries();
}

void EditorWorld::InitializeQueries()
{
    duin::GameWorld::InitializeQueries();
    queryEntityPositionGizmos = this->QueryBuilder<const duin::ECSComponent::Transform3D>().Cached().Build();
}

void EditorWorld::PostUpdateQueryExecution(double delta)
{
}

void EditorWorld::PostPhysicsUpdateQueryExecution(double delta)
{
    ExecuteQueryTransform3DHierarchicalUpdate();
    ExecuteQueryCharacterBody3DUpdateTransform();
    //ExecuteQueryControlCamera();
    //ExecuteQuerySetCameraAsActive();
}

void EditorWorld::PostDrawQueryExecution()
{
    ExecuteQueryDrawCube();
    ExecuteQueryDrawDebugCapsule();
    ExecuteQueryDrawDebugCube();
    ExecuteQueryDrawTransform3DGizmos();
}

void EditorWorld::PostDrawUIQueryExecution()
{
}

void EditorWorld::ExecuteQueryDrawTransform3DGizmos()
{
    // Draw position marker for every entity with a Transform3D
    queryEntityPositionGizmos.Each([](duin::Entity e, const duin::ECSComponent::Transform3D &tx) {
        duin::Vector3 gPos = tx.GetGlobalPosition(e.GetFlecsEntity());
        duin::DrawDebugSphere(gPos, 1.0f);
    });
}

void EditorWorld::ExecuteQuerySyncCharacterBodyWithTransform3D()
{
    querySyncCharacterBodyWithTransform3D.Each(
        [](duin::Entity e, duin::ECSComponent::CharacterBodyComponent &cb, duin::ECSComponent::Transform3D &tx) {
            if (!cb.body)
            {
                DN_WARN("CharacterBody pointer is nullptr!");
                return;
            }
            cb.body->SetPosition(tx.GetGlobalPosition());
        });
}
