#include "dnpch.h"
#include "GameWorld.h"
#include "ComponentInspector.h"
#include "Duin/Core/Application.h"
#include "Duin/Core/Utils/SerialisationManager.h"
#include "Duin/Render/Camera.h"
#include "Duin/Render/Renderer.h"
#include "PrefabRegistry.h"
#include "Duin/Core/Debug/DNLog.h"

#include <functional>

namespace duin
{

/*----------------------------------------------------------------------------*/
//  GameWorld Functions
/*----------------------------------------------------------------------------*/
GameWorld::GameWorld()
{
}

GameWorld::GameWorld(bool connectSignals)
{
    Initialize(connectSignals);
}

GameWorld::~GameWorld()
{
}

void GameWorld::Initialize(bool connectSignals)
{
    ECSTag::RegisterTags(*this);
    DN_CORE_INFO("Tags registered.");
    ECSComponent::RegisterComponents(*this);
    DN_CORE_INFO("Components registered.");
    ECSPrefab::RegisterPrefabs(*this);
    DN_CORE_INFO("Prefabs registered.");
    ECSObserver::RegisterObservers(*this);
    DN_CORE_INFO("Observers registered.");

    if (connectSignals)
    {
        connPostUpdate_ = QueuePostUpdateCallback([this](double delta) { PostUpdateQueryExecution(delta); });
        connPostPhysicsUpdate_ =
            QueuePostPhysicsUpdateCallback([this](double delta) { PostPhysicsUpdateQueryExecution(delta); });
        connPostDraw_ = QueuePostDrawCallback([this]() { PostDrawQueryExecution(); });
        connPostDrawUI_ = QueuePostDrawUICallback([this]() { PostDrawUIQueryExecution(); });
    }
}

void GameWorld::ActivateCameraEntity(duin::Entity entity)
{
    if (!entity.IsValid())
    {
        DN_CORE_FATAL("Entity invalid!");
        return;
    }
    else if (entity.Has<Camera>())
    {
        this->DeferBegin();
        this->GetFlecsWorld().each([](flecs::entity e, ECSTag::ActiveCamera) { e.remove<ECSTag::ActiveCamera>(); });
        entity.Add<ECSTag::ActiveCamera>();
        this->DeferEnd();
    }
}

void GameWorld::PostUpdateQueryExecution(double delta)
{
}

void GameWorld::PostPhysicsUpdateQueryExecution(double delta)
{

    Progress(); // TODO testing for remote viewing
}

void GameWorld::PostDrawQueryExecution()
{

}

void GameWorld::PostDrawUIQueryExecution()
{
}

void GameWorld::Clear()
{
    // Clear the query cache before resetting the world — queries hold pointers
    // into the old ecs_world_t* and must not outlive it.
    ClearQueryCache();

    this->GetFlecsWorld().reset();
}

void GameWorld::Reset(bool connectSignals)
{
    Clear();
    Initialize(connectSignals);
}

void GameWorld::InitializeRemoteExplorer()
{
    // Remote viewing
     GetFlecsWorld().import <flecs::stats>();
     GetFlecsWorld().set<flecs::Rest>({});
    //while (GetFlecsWorld().progress())
    //{
    //}

    //GetFlecsWorld()
    //    .app()
    //    // Optional, gather statistics for explorer
    //    .enable_stats()
    //    .enable_rest()
    //    .run();
}

/*----------------------------------------------------------------------
 * Global-space transform helpers
----------------------------------------------------------------------*/
void GameWorld::SetGlobalTransform(duin::Entity e, ECSComponent::Transform3D tx)
{
    if (!e.IsValid() || !e.Has<ECSComponent::Transform3D>())
    {
        DN_CORE_WARN("Entity not valid, or does not have Transform3D!");
        return;
    }
    ECSComponent::Transform3D *comp = e.TryGetMut<ECSComponent::Transform3D>();
    if (!comp)
    {
        return;
    }

    duin::Entity parent = e.Parent();
    if (parent.IsValid() && parent.Has<ECSComponent::Transform3D>())
    {
        ECSComponent::Transform3D parentGlobal = GetGlobalTransform(parent);

        Vector3 offset = Vector3Subtract(tx.GetPosition(), parentGlobal.GetPosition());
        Quaternion invParentRot = QuaternionInvert(parentGlobal.GetRotation());
        Vector3 localPosUnscaled = Vector3RotateByQuaternion(offset, invParentRot);
        Vector3 localPos = Vector3Divide(localPosUnscaled, parentGlobal.GetScale());

        Vector3 localScale = Vector3Divide(tx.GetScale(), parentGlobal.GetScale());

        Quaternion localRot = QuaternionMultiply(QuaternionInvert(parentGlobal.GetRotation()), tx.GetRotation());

        comp->SetPosition(localPos);
        comp->SetScale(localScale);
        comp->SetRotation(localRot);
    }
    else
    {
        comp->SetPosition(tx.GetPosition());
        comp->SetScale(tx.GetScale());
        comp->SetRotation(tx.GetRotation());
    }
    comp->UpdateGlobalPositionCache(tx.GetPosition());
    comp->UpdateGlobalScaleCache(tx.GetScale());
    comp->UpdateGlobalRotationCache(tx.GetRotation());
}

ECSComponent::Transform3D GameWorld::GetGlobalTransform(duin::Entity e)
{
    if (!e.IsValid() || !e.Has<ECSComponent::Transform3D>())
    {
        return ECSComponent::Transform3D();
    }
    ECSComponent::Transform3D *tx = e.TryGetMut<ECSComponent::Transform3D>();
    if (!tx)
    {
        return ECSComponent::Transform3D();
    }

    duin::Entity parent = e.Parent();
    if (parent.IsValid() && parent.Has<ECSComponent::Transform3D>())
    {
        ECSComponent::Transform3D parentGlobal = GetGlobalTransform(parent);

        Vector3 scaledLocalPos = Vector3Multiply(tx->GetPosition(), parentGlobal.GetScale());
        Vector3 rotatedPos = Vector3RotateByQuaternion(scaledLocalPos, parentGlobal.GetRotation());
        Vector3 globalPos = Vector3Add(parentGlobal.GetPosition(), rotatedPos);

        Vector3 globalScale = Vector3Multiply(parentGlobal.GetScale(), tx->GetScale());

        Quaternion globalRot = QuaternionMultiply(parentGlobal.GetRotation(), tx->GetRotation());

        tx->UpdateGlobalPositionCache(globalPos);
        tx->UpdateGlobalScaleCache(globalScale);
        tx->UpdateGlobalRotationCache(globalRot);

        return ECSComponent::Transform3D(globalPos, globalScale, globalRot);
    }
    else
    {
        tx->UpdateGlobalPositionCache(tx->GetPosition());
        tx->UpdateGlobalScaleCache(tx->GetScale());
        tx->UpdateGlobalRotationCache(tx->GetRotation());

        return *tx;
    }
}

void GameWorld::SetGlobalPosition(duin::Entity e, Vector3 position)
{
    if (!e.IsValid() || !e.Has<ECSComponent::Transform3D>())
    {
        DN_CORE_WARN("Entity not valid, or does not have Transform3D!");
        return;
    }
    ECSComponent::Transform3D *tx = e.TryGetMut<ECSComponent::Transform3D>();
    if (!tx)
        return;

    duin::Entity parent = e.Parent();
    if (parent.IsValid() && parent.Has<ECSComponent::Transform3D>())
    {
        Vector3 parentGlobalPos = GetGlobalPosition(parent);
        Quaternion parentGlobalRot = GetGlobalRotation(parent);
        Vector3 parentGlobalScale = GetGlobalScale(parent);
        Vector3 offset = Vector3Subtract(position, parentGlobalPos);
        Quaternion invParentRot = QuaternionInvert(parentGlobalRot);
        Vector3 localPosUnscaled = Vector3RotateByQuaternion(offset, invParentRot);
        Vector3 localPos = Vector3Divide(localPosUnscaled, parentGlobalScale);
        tx->SetPosition(localPos);
    }
    else
    {
        tx->SetPosition(position);
    }
    tx->UpdateGlobalPositionCache(position);
}

Vector3 GameWorld::GetGlobalPosition(duin::Entity e)
{
    if (!e.IsValid() || !e.Has<ECSComponent::Transform3D>())
    {
        //DN_CORE_WARN("Entity not valid, or does not have Transform3D!");
        return Vector3Zero();
    }
    ECSComponent::Transform3D *tx = e.TryGetMut<ECSComponent::Transform3D>();
    if (!tx)
    {
        return Vector3Zero();
    }

    if (tx->globalPositionCacheDirtyFlag)
    {
        duin::Entity parent = e.Parent();
        if (!parent.IsValid() || !parent.Has<ECSComponent::Transform3D>())
        {
            tx->UpdateGlobalPositionCache(tx->GetPosition());
        }
        else
        {
            Vector3 parentGlobalPos = GetGlobalPosition(parent);
            Quaternion parentGlobalRot = GetGlobalRotation(parent);
            Vector3 parentGlobalScale = GetGlobalScale(parent);
            Vector3 scaledLocalPos = Vector3Multiply(tx->GetPosition(), parentGlobalScale);
            Vector3 rotatedPos = Vector3RotateByQuaternion(scaledLocalPos, parentGlobalRot);
            Vector3 globalPos = Vector3Add(parentGlobalPos, rotatedPos);
            tx->UpdateGlobalPositionCache(globalPos);
        }
    }

    return tx->globalPositionCache;
}

void GameWorld::SetGlobalScale(duin::Entity e, Vector3 scale)
{
    if (!e.IsValid() || !e.Has<ECSComponent::Transform3D>())
    {
        return;
    }
    ECSComponent::Transform3D *tx = e.TryGetMut<ECSComponent::Transform3D>();
    if (!tx)
    {
        return;
    }

    duin::Entity parent = e.Parent();
    if (parent.IsValid() && parent.Has<ECSComponent::Transform3D>())
    {
        Vector3 parentGlobalScale = GetGlobalScale(parent);
        tx->SetScale(Vector3Divide(scale, parentGlobalScale));
    }
    else
    {
        tx->SetScale(scale);
    }
    tx->UpdateGlobalScaleCache(scale);
}

Vector3 GameWorld::GetGlobalScale(duin::Entity e)
{
    if (!e.IsValid() || !e.Has<ECSComponent::Transform3D>())
    {
        return Vector3One();
    }
    ECSComponent::Transform3D *tx = e.TryGetMut<ECSComponent::Transform3D>();
    if (!tx)
    {
        return Vector3One();
    }

    if (tx->globalScaleCacheDirtyFlag)
    {
        duin::Entity parent = e.Parent();
        if (!parent.IsValid() || !parent.Has<ECSComponent::Transform3D>())
        {
            tx->UpdateGlobalScaleCache(tx->GetScale());
        }
        else
        {
            Vector3 parentGlobalScale = GetGlobalScale(parent);
            Vector3 globalScale = Vector3Multiply(parentGlobalScale, tx->GetScale());
            tx->UpdateGlobalScaleCache(globalScale);
        }
    }

    return tx->globalScaleCache;
}

void GameWorld::SetGlobalRotation(duin::Entity e, Quaternion rotation)
{
    if (!e.IsValid() || !e.Has<ECSComponent::Transform3D>())
    {
        DN_CORE_WARN("Entity not valid, or does not have Transform3D!");
        return;
    }
    ECSComponent::Transform3D *tx = e.TryGetMut<ECSComponent::Transform3D>();
    if (!tx)
    {
        return;
    }

    duin::Entity parent = e.Parent();
    if (parent.IsValid() && parent.Has<ECSComponent::Transform3D>())
    {
        Quaternion parentGlobalRot = GetGlobalRotation(parent);
        Quaternion invParentRot = QuaternionInvert(parentGlobalRot);
        Quaternion localRotation = QuaternionMultiply(invParentRot, rotation);
        tx->SetRotation(localRotation);
    }
    else
    {
        tx->SetRotation(rotation);
    }
    tx->UpdateGlobalRotationCache(rotation);
}

Quaternion GameWorld::GetGlobalRotation(duin::Entity e)
{
    if (!e.IsValid() || !e.Has<ECSComponent::Transform3D>())
    {
        DN_CORE_WARN("Entity not valid, or does not have Transform3D!");
        return QuaternionIdentity();
    }
    ECSComponent::Transform3D *tx = e.TryGetMut<ECSComponent::Transform3D>();
    if (!tx)
    {
        return QuaternionIdentity();
    }

    if (tx->globalRotationCacheDirtyFlag)
    {
        duin::Entity parent = e.Parent();
        if (!parent.IsValid() || !parent.Has<ECSComponent::Transform3D>())
        {
            tx->UpdateGlobalRotationCache(tx->GetRotation());
        }
        else
        {
            Quaternion parentGlobalRot = GetGlobalRotation(parent);
            Quaternion globalRot = QuaternionMultiply(parentGlobalRot, tx->GetRotation());
            tx->UpdateGlobalRotationCache(globalRot);
        }
    }
    return tx->globalRotationCache;
}

} // namespace duin
