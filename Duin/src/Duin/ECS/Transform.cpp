#include "dnpch.h"
#include "Transform.h"

void duin::transform::SetGlobalTransform(duin::Entity e, ECSComponent::Transform3D tx)
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
    comp->_UpdateGlobalPositionCache(tx.GetPosition());
    comp->_UpdateGlobalScaleCache(tx.GetScale());
    comp->_UpdateGlobalRotationCache(tx.GetRotation());
}

duin::ECSComponent::Transform3D duin::transform::GetGlobalTransform(duin::Entity e)
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

        tx->_UpdateGlobalPositionCache(globalPos);
        tx->_UpdateGlobalScaleCache(globalScale);
        tx->_UpdateGlobalRotationCache(globalRot);

        return ECSComponent::Transform3D(globalPos, globalScale, globalRot);
    }
    else
    {
        tx->_UpdateGlobalPositionCache(tx->GetPosition());
        tx->_UpdateGlobalScaleCache(tx->GetScale());
        tx->_UpdateGlobalRotationCache(tx->GetRotation());

        return *tx;
    }
}

void duin::transform::SetGlobalPosition(duin::Entity e, Vector3 position)
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
    tx->_UpdateGlobalPositionCache(position);
}

duin::Vector3 duin::transform::GetGlobalPosition(duin::Entity e)
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

    if (tx->_GetGlobalPositionCacheDirtyFlag())
    {
        duin::Entity parent = e.Parent();
        if (!parent.IsValid() || !parent.Has<ECSComponent::Transform3D>())
        {
            tx->_UpdateGlobalPositionCache(tx->GetPosition());
        }
        else
        {
            Vector3 parentGlobalPos = GetGlobalPosition(parent);
            Quaternion parentGlobalRot = GetGlobalRotation(parent);
            Vector3 parentGlobalScale = GetGlobalScale(parent);
            Vector3 scaledLocalPos = Vector3Multiply(tx->GetPosition(), parentGlobalScale);
            Vector3 rotatedPos = Vector3RotateByQuaternion(scaledLocalPos, parentGlobalRot);
            Vector3 globalPos = Vector3Add(parentGlobalPos, rotatedPos);
            tx->_UpdateGlobalPositionCache(globalPos);
        }
    }

    return tx->_GetGlobalPositionCache();
}

void duin::transform::SetGlobalScale(duin::Entity e, Vector3 scale)
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
    tx->_UpdateGlobalScaleCache(scale);
}

duin::Vector3 duin::transform::GetGlobalScale(duin::Entity e)
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

    if (tx->_GetGlobalScaleCacheDirtyFlag())
    {
        duin::Entity parent = e.Parent();
        if (!parent.IsValid() || !parent.Has<ECSComponent::Transform3D>())
        {
            tx->_UpdateGlobalScaleCache(tx->GetScale());
        }
        else
        {
            Vector3 parentGlobalScale = GetGlobalScale(parent);
            Vector3 globalScale = Vector3Multiply(parentGlobalScale, tx->GetScale());
            tx->_UpdateGlobalScaleCache(globalScale);
        }
    }

    return tx->_GetGlobalScaleCache();
}

void duin::transform::SetGlobalRotation(duin::Entity e, Quaternion rotation)
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
    tx->_UpdateGlobalRotationCache(rotation);
}

duin::Quaternion duin::transform::GetGlobalRotation(duin::Entity e)
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

    if (tx->_GetGlobalRotationCacheDirtyFlag())
    {
        duin::Entity parent = e.Parent();
        if (!parent.IsValid() || !parent.Has<ECSComponent::Transform3D>())
        {
            tx->_UpdateGlobalRotationCache(tx->GetRotation());
        }
        else
        {
            Quaternion parentGlobalRot = GetGlobalRotation(parent);
            Quaternion globalRot = QuaternionMultiply(parentGlobalRot, tx->GetRotation());
            tx->_UpdateGlobalRotationCache(globalRot);
        }
    }
    return tx->_GetGlobalRotationCache();
}
