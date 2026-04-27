#include "EditorWorld.h"
#include <Duin/Render/RenderModule.h>

EditorWorld::EditorWorld()
{
}

EditorWorld::~EditorWorld()
{
}

void EditorWorld::PostUpdateQueryExecution(double delta)
{
}

void EditorWorld::PostPhysicsUpdateQueryExecution(double delta)
{
    //ExecuteQueryTransform3DHierarchicalUpdate();
    //ExecuteQueryCharacterBody3DUpdateTransform();
    // ExecuteQueryControlCamera();
    // ExecuteQuerySetCameraAsActive();
}

void EditorWorld::PostDrawQueryExecution()
{
    //ExecuteQueryDrawCube();
    //ExecuteQueryDrawDebugCapsule();
    //ExecuteQueryDrawDebugCube();
    ExecuteQueryDrawTransform3DGizmos();
}

void EditorWorld::PostDrawUIQueryExecution()
{
}

void EditorWorld::ExecuteQueryDrawTransform3DGizmos()
{
    // clang-format off
    // Draw position marker for every entity with a Transform3D
    GetOrBuildQuery<
            const duin::ECSComponent::Transform3D
    >(
        "DrawTransform3DGizmos", 
        [](duin::World& w) 
        {
            return w.QueryBuilder<
                    const duin::ECSComponent::Transform3D
                >()
                .Cached()
                .Build();
        }
    ).Each([this](
        duin::Entity e,
        const duin::ECSComponent::Transform3D &tx
    ){
        duin::Vector3 gPos = GetGlobalPosition(e);
        duin::DrawDebugSphere(gPos, 1.5f);
    });
    // clang-format on
}

void EditorWorld::ExecuteQuerySyncCharacterBodyWithTransform3D()
{
    // clang-format off
    GetOrBuildQuery<
        //duin::ECSComponent::CharacterBodyComponent,
        duin::ECSComponent::Transform3D
    >(
        "SyncCharacterBodyWithTransform3D", 
        [](duin::World& w) 
        {
            return w.QueryBuilder<
                    //duin::ECSComponent::CharacterBodyComponent,
                    duin::ECSComponent::Transform3D
                >()
                .Cached()
                .Build();
        }
    ).Each([this](
        duin::Entity e,
        //duin::ECSComponent::CharacterBodyComponent &cb,
        duin::ECSComponent::Transform3D &tx
    ){
        //if (!cb.body)
        //{
        //    DN_WARN("CharacterBody pointer is nullptr!");
        //    return;
        //}
        //cb.body->SetPosition(GetGlobalPosition(e));
    });
    // clang-format on
}
