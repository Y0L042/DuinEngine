/**
 * @file GameWorld.h
 * @brief Entity Component System manager and component definitions.
 * @ingroup ECS
 *
 * Central hub for the FLECS-based ECS. Contains:
 * - ECSTag namespace: Tag components for entity categorization
 * - ECSComponent namespace: Data components
 * - ECSPrefab namespace: Entity templates
 * - GameWorld class: World management and query execution
 */

#pragma once

#include <memory.h>

#include <flecs.h>
#include <rfl.hpp>
#include "Duin/ECS/DECS/DECS.h"
#include "Duin/Core/Maths/DuinMaths.h"
#include "Duin/Core/Debug/DNLog.h"
#include "Duin/Core/Utils/UUID.h"
#include "Duin/Physics/PhysicsIncludes.h"
#include "Duin/Render/Camera.h"

namespace duin
{

/**
 * @namespace ECSTag
 * @brief Tag components for entity categorization.
 * @ingroup ECS_Tags
 *
 * Tags are empty structs used to mark entities. They enable filtering
 * in queries without storing data.
 */
namespace ECSTag
{
/** @brief Marks transform as local (relative to parent). */
struct Local
{
};
/** @brief Marks transform as global (world space). */
struct Global
{
};

/** @brief Queues entity for external reference creation. */
struct CreateExternalRef
{
};
/** @brief Indicates entity has active external references. */
struct ActiveExternalRef
{
};
/** @brief Queues entity for external reference deletion. */
struct DeleteExternalRef
{
};

/** @brief Marks entity as having a kinematic physics body. */
struct PxKinematic
{
};
/** @brief Marks entity as having a dynamic physics body. */
struct PxDynamic
{
};
/** @brief Marks entity as having a static physics body. */
struct PxStatic
{
};
/** @brief Marks entity as not having a physics body. */
struct NonPx
{
};

/** @brief Request to set this camera as active. */
struct SetCameraAsActive
{
};
/** @brief Indicates camera is currently active. */
struct CameraIsActive
{
};
/** @brief Singleton tag for the active camera entity. */
struct ActiveCamera
{
};
}; // namespace ECSTag

/**
 * @namespace ECSComponent
 * @brief Data components for entities.
 * @ingroup ECS_Components
 *
 * Components are pure data structures attached to entities.
 * They define entity properties like position, rotation, velocity,
 * physics bodies, and visual appearance.
 */
namespace ECSComponent
{

/**
 * @name 2D Components
 * Components for 2D transforms and movement.
 * @{
 */

/** @brief 2D position component. */
struct Position2D
{
    Vector2 value;

    Position2D() : value(Vector2Zero())
    {
    }

    Position2D(Vector2 value) : value(value)
    {
    }

    Position2D(float x, float y) : value(x, y)
    {
    }

    struct Position2DImpl
    {
        Vector2 v;
    };
    using ReflectionType = Position2DImpl;
    Position2D(const ReflectionType &impl) : value(impl.v)
    {
    }
    const ReflectionType &reflection() const
    {
        return Position2DImpl{value};
    }
};

/** @brief 2D rotation component (angle in radians). */
struct Rotation2D
{
    float value;

    Rotation2D() : value(0.0f)
    {
    }

    Rotation2D(float r) : value(r)
    {
    }

    struct Rotation2DImpl
    {
        float v;
    };
    using ReflectionType = Rotation2DImpl;
    Rotation2D(const ReflectionType &impl) : value(impl.v)
    {
    }
    const ReflectionType &reflection() const
    {
        return Rotation2DImpl{value};
    }
};

/** @brief 2D scale component. */
struct Scale2D
{
    Vector2 value;

    Scale2D() : value(Vector2Zero())
    {
    }

    Scale2D(Vector2 value) : value(value)
    {
    }

    Scale2D(float x, float y) : value(x, y)
    {
    }

    struct Scale2DImpl
    {
        Vector2 v;
    };
    using ReflectionType = Scale2DImpl;
    Scale2D(const ReflectionType &impl) : value(impl.v)
    {
    }
    const ReflectionType &reflection() const
    {
        return Scale2DImpl{value};
    }
};

/** @brief Composite 2D transform marker. */
struct Transform2D
{
};
/** @} */

/**
 * @struct Velocity2D
 * @brief 2D velocity component (always in global coordinates).
 */
struct Velocity2D
{
    Vector2 value;

    Velocity2D() : value(Vector2Zero())
    {
    }

    Velocity2D(Vector2 value) : value(value)
    {
    }

    Velocity2D(float x, float y) : value(x, y)
    {
    }

    struct Velocity2DImpl
    {
        Vector2 v;
    };
    using ReflectionType = Velocity2DImpl;
    Velocity2D(const ReflectionType &impl) : value(impl.v)
    {
    }
    const ReflectionType &reflection() const
    {
        return Velocity2DImpl{value};
    }
};

/**
 * @name 3D Transform Components
 * Components for 3D transforms.
 * @{
 */

/** @brief 3D position component. */
struct Position3D
{
    Vector3 value;

    Position3D() : value(Vector3Zero())
    {
    }

    Position3D(Vector3 value) : value(value)
    {
    }

    Position3D(float x, float y, float z) : value(x, y, z)
    {
    }

    struct Position3DImpl
    {
        Vector3 v;
    };
    using ReflectionType = Position3DImpl;
    Position3D(const ReflectionType &impl) : value(impl.v)
    {
    }
    const ReflectionType &reflection() const
    {
        return Position3DImpl{value};
    }
};

/** @brief 3D rotation component (quaternion). */
struct Rotation3D
{
    Quaternion value;

    Rotation3D() : value(Vector4Zero())
    {
    }

    Rotation3D(Quaternion value) : value(value)
    {
    }

    Rotation3D(float x, float y, float z, float w) : value(x, y, z, w)
    {
    }

    struct Rotation3DImpl
    {
        Quaternion v;
    };
    using ReflectionType = Rotation3DImpl;
    Rotation3D(const ReflectionType &impl) : value(impl.v)
    {
    }
    const ReflectionType &reflection() const
    {
        return Rotation3DImpl{value};
    }
};

/** @brief 3D scale component. */
struct Scale3D
{
    Vector3 value;

    Scale3D() : value(Vector3Zero())
    {
    }

    Scale3D(Vector3 value) : value(value)
    {
    }

    Scale3D(float x, float y, float z) : value(x, y, z)
    {
    }

    struct Scale3DImpl
    {
        Vector3 v;
    };
    using ReflectionType = Scale3DImpl;
    Scale3D(const ReflectionType &impl) : value(impl.v)
    {
    }
    const ReflectionType &reflection() const
    {
        return Scale3DImpl{value};
    }
};

/**
 * @struct Transform3D
 * @brief Hierarchical 3D transform with caching.
 *
 * Combines position, rotation, and scale with automatic global transform
 * computation through the entity hierarchy. Uses dirty flags for
 * efficient cache invalidation.
 *
 * Local values are relative to parent. Use GetGlobal/SetGlobal methods
 * for world-space operations.
 */
struct Transform3D
{
  public:
    static void RegisterOnSetObserver(flecs::world &world)
    {
        DN_CORE_INFO("Register Transform3D OnSet Observer.");
        static flecs::observer o = world.observer<Transform3D>()
                                       .event(flecs::OnSet)
                                       .observer_flags(EcsObserverYieldOnCreate)
                                       .each([](flecs::entity e, Transform3D &tx) { tx.SetEntity(e); });
    }

    static void RegisterOnAddObserver(flecs::world &world)
    {
        DN_CORE_INFO("Register Transform3D OnAdd Observer.");
        static flecs::observer o = world.observer<Transform3D>()
                                       .event(flecs::OnAdd)
                                       .observer_flags(EcsObserverYieldOnCreate)
                                       .each([](flecs::entity e, Transform3D &tx) { tx.SetEntity(e); });
    }

    static void SetGlobalTransform(flecs::entity e, Transform3D tx)
    {
        if (!e.is_valid() || !e.has<Transform3D>())
        {
            DN_CORE_WARN("STATIC Entity not valid, or does not have Transform3D!");
            return;
        }
        Transform3D *comp = e.try_get_mut<Transform3D>();
        if (!comp)
        {
            return;
        }

        flecs::entity parent = e.parent();
        if (parent.is_valid() && parent.has<Transform3D>())
        {
            Transform3D parentGlobal = GetGlobalTransform(parent);

            // Compute local position.
            // globalPos = parentGlobal.position + rotate(parentGlobal.rotation, localPos * parentGlobal.scale)
            // Thus: localPos = inverse(parentGlobal.rotation) * (globalPos - parentGlobal.position) /
            // parentGlobal.scale
            Vector3 offset = Vector3Subtract(tx.GetPosition(), parentGlobal.GetPosition());
            Quaternion invParentRot = QuaternionInvert(parentGlobal.GetRotation());
            Vector3 localPosUnscaled = Vector3RotateByQuaternion(offset, invParentRot);
            Vector3 localPos = Vector3Divide(localPosUnscaled, parentGlobal.GetScale());

            // Compute local scale.
            // Assuming scale is applied component-wise:
            Vector3 localScale = Vector3Divide(tx.GetScale(), parentGlobal.GetScale());

            // Compute local rotation.
            // globalRot = parentGlobal.rotation * localRot, so:
            Quaternion localRot = QuaternionMultiply(QuaternionInvert(parentGlobal.GetRotation()), tx.GetRotation());

            comp->SetPosition(localPos);
            comp->SetScale(localScale);
            comp->SetRotation(localRot);
        }
        else
        {
            // No parent: local transform equals desired global transform.
            comp->SetPosition(tx.GetPosition());
            comp->SetScale(tx.GetScale());
            comp->SetRotation(tx.GetRotation());
        }
        // Update caches immediately.
        comp->UpdateGlobalPositionCache(tx.GetPosition());
        comp->UpdateGlobalScaleCache(tx.GetScale());
        comp->UpdateGlobalRotationCache(tx.GetRotation());
    }

    static Transform3D GetGlobalTransform(flecs::entity e)
    {
        if (!e.is_valid() || !e.has<Transform3D>())
        {
            return Transform3D();
        }
        Transform3D *tx = e.try_get_mut<Transform3D>();
        if (!tx)
        {
            return Transform3D();
        }

        flecs::entity parent = e.parent();
        if (parent.is_valid() && parent.has<Transform3D>())
        {
            // Get parent's global transform recursively.
            Transform3D parentGlobal = GetGlobalTransform(parent);

            // Compute global position:
            // globalPos = parentGlobal.position + rotate(parentGlobal.rotation, localPos * parentGlobal.scale)
            Vector3 scaledLocalPos = Vector3Multiply(tx->GetPosition(), parentGlobal.GetScale());
            Vector3 rotatedPos = Vector3RotateByQuaternion(scaledLocalPos, parentGlobal.GetRotation());
            Vector3 globalPos = Vector3Add(parentGlobal.GetPosition(), rotatedPos);

            // Compute global scale.
            Vector3 globalScale = Vector3Multiply(parentGlobal.GetScale(), tx->GetScale());

            // Compute global rotation.
            Quaternion globalRot = QuaternionMultiply(parentGlobal.GetRotation(), tx->GetRotation());

            // Update the caches.
            tx->UpdateGlobalPositionCache(globalPos);
            tx->UpdateGlobalScaleCache(globalScale);
            tx->UpdateGlobalRotationCache(globalRot);

            return Transform3D(globalPos, globalRot, e);
        }
        else
        {
            // No parent: global transform equals local transform.
            tx->UpdateGlobalPositionCache(tx->GetPosition());
            tx->UpdateGlobalScaleCache(tx->GetScale());
            tx->UpdateGlobalRotationCache(tx->GetRotation());

            return *tx;
        }
    }

    static void SetGlobalPosition(flecs::entity e, Vector3 position)
    {
        if (!e.is_valid() || !e.has<Transform3D>())
        {
            DN_CORE_WARN("STATIC Entity not valid, or does not have Transform3D!");
            return;
        }
        Transform3D *tx = e.try_get_mut<Transform3D>();
        if (!tx)
            return;

        flecs::entity parent = e.parent();
        if (parent.is_valid() && parent.has<Transform3D>())
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

    static Vector3 GetGlobalPosition(flecs::entity e)
    {
        if (!e.is_valid() || !e.has<Transform3D>())
        {
            DN_CORE_WARN("STATIC Entity not valid, or does not have Transform3D!");
            return Vector3Zero();
        }
        Transform3D *tx = e.try_get_mut<Transform3D>();
        if (!tx)
        {
            return Vector3Zero();
        }

        if (tx->globalPositionCacheDirtyFlag)
        {
            flecs::entity parent = e.parent();
            if (!parent.is_valid() || !parent.has<Transform3D>())
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

    static void SetGlobalScale(flecs::entity e, Vector3 scale)
    {
        if (!e.is_valid() || !e.has<Transform3D>())
        {
            return;
        }
        Transform3D *tx = e.try_get_mut<Transform3D>();
        if (!tx)
        {
            return;
        }

        flecs::entity parent = e.parent();
        if (parent.is_valid() && parent.has<Transform3D>())
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

    static Vector3 GetGlobalScale(flecs::entity e)
    {
        if (!e.is_valid() || !e.has<Transform3D>())
        {
            return Vector3One();
        }
        Transform3D *tx = e.try_get_mut<Transform3D>();
        if (!tx)
        {
            return Vector3One();
        }

        if (tx->globalScaleCacheDirtyFlag)
        {
            if (!e.parent().is_valid() || !e.parent().has<Transform3D>())
            {
                tx->UpdateGlobalScaleCache(tx->GetScale());
            }
            else
            {
                Vector3 parentGlobalScale = GetGlobalScale(e.parent());
                Vector3 globalScale = Vector3Multiply(parentGlobalScale, tx->GetScale());
                tx->UpdateGlobalScaleCache(globalScale);
            }
        }

        return tx->globalScaleCache;
    }

    static void SetGlobalRotation(flecs::entity e, Quaternion rotation)
    {
        if (!e.is_valid() || !e.has<Transform3D>())
        {
            DN_CORE_WARN("STATIC Entity not valid, or does not have Transform3D!");
            return;
        }
        Transform3D *tx = e.try_get_mut<Transform3D>();
        if (!tx)
        {
            return;
        }

        flecs::entity parent = e.parent();
        if (parent.is_valid() && parent.has<Transform3D>())
        {
            // Global = ParentGlobal * Local  =>  Local = inverse(ParentGlobal) * Global
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

    static Quaternion GetGlobalRotation(flecs::entity e)
    {
        if (!e.is_valid() || !e.has<Transform3D>())
        {
            DN_CORE_WARN("STATIC Entity not valid, or does not have Transform3D!");
            return QuaternionIdentity();
        }
        Transform3D *tx = e.try_get_mut<Transform3D>();
        if (!tx)
        {
            return QuaternionIdentity();
        }

        if (tx->globalRotationCacheDirtyFlag)
        {
            flecs::entity parent = e.parent();
            if (!parent.is_valid() || !parent.has<Transform3D>())
            {
                // No parent: global rotation is the local rotation.
                tx->UpdateGlobalRotationCache(tx->GetRotation());
            }
            else
            {
                // Global = ParentGlobal * LocalRotation
                Quaternion parentGlobalRot = GetGlobalRotation(parent);
                Quaternion globalRot = QuaternionMultiply(parentGlobalRot, tx->GetRotation());
                tx->UpdateGlobalRotationCache(globalRot);
            }
        }
        return tx->globalRotationCache;
    }

    Transform3D(Vector3 position = Vector3Zero(), Vector3 scale = Vector3One(),
                Quaternion rotation = QuaternionIdentity())
        : position_(position), scale_(scale), rotation_(rotation) {};

    Transform3D(Vector3 position, Quaternion rotation)
        : position_(position), scale_(Vector3One()), rotation_(rotation) {};

    Transform3D(Vector3 position, Quaternion rotation, flecs::entity e)
        : position_(position), scale_(Vector3One()), rotation_(rotation), entity_(e) {};

    Transform3D(Vector3 position, flecs::entity e)
        : position_(position), scale_(Vector3One()), rotation_(QuaternionIdentity()), entity_(e) {};

    Transform3D(flecs::entity e)
        : position_(Vector3Zero()), scale_(Vector3One()), rotation_(QuaternionIdentity()), entity_(e)
    {
    }

    Transform3D(const Transform3D &tx)
        : position_(tx.GetPosition()), scale_(tx.GetScale()), rotation_(tx.GetRotation()), entity_(tx.GetEntity()) {};

    void InvalidateCacheFlags()
    {
        globalPositionCacheDirtyFlag = true;
        globalScaleCacheDirtyFlag = true;
        globalRotationCacheDirtyFlag = true;

        PropagateInvalidateCacheFlags();
    }

    void SetEntity(flecs::entity e)
    {
        entity_ = e;
    }

    flecs::entity GetEntity() const
    {
        return entity_;
    }

    void SetPosition(Vector3 position)
    {
        position_ = position;
        InvalidateCacheFlags();
    }

    Vector3 GetPosition() const
    {
        return position_;
    }

    void SetScale(Vector3 scale)
    {
        scale_ = scale;
        InvalidateCacheFlags();
    }

    Vector3 GetScale() const
    {
        return scale_;
    }

    void SetRotation(Quaternion rotation)
    {
        rotation_ = rotation;
        InvalidateCacheFlags();
    }

    Quaternion GetRotation() const
    {
        return rotation_;
    }

    void SetGlobalTransform(Transform3D tx)
    {
        if (!entity_.is_valid() || !entity_.has<Transform3D>())
        {
            DN_CORE_WARN("Entity not valid, or does not have Transform3D!");
            return;
        }
        SetGlobalTransform(entity_, tx);
    }

    Transform3D GetGlobalTransform() const
    {
        if (!entity_.is_valid() || !entity_.has<Transform3D>())
        {
            DN_CORE_WARN("Entity not valid, or does not have Transform3D!");
            return Transform3D();
        }
        GetGlobalTransform(entity_);
    }

    void SetGlobalPosition(Vector3 position)
    {
        if (!entity_.is_valid() || !entity_.has<Transform3D>())
        {
            DN_CORE_WARN("Entity not valid, or does not have Transform3D!");
            return;
        }
        SetGlobalPosition(entity_, position);
    }

    Vector3 GetGlobalPosition() const
    {
        if (!entity_.is_valid() || !entity_.has<Transform3D>())
        {
            DN_CORE_WARN("Entity not valid, or does not have Transform3D!");
            return Vector3Zero();
        }
        return GetGlobalPosition(entity_);
    }

    void SetGlobalScale(Vector3 scale)
    {
        if (!entity_.is_valid() || !entity_.has<Transform3D>())
        {
            DN_CORE_WARN("Entity not valid, or does not have Transform3D!");
            return;
        }
        SetGlobalScale(entity_, scale);
    }

    Vector3 GetGlobalScale() const
    {
        if (!entity_.is_valid() || !entity_.has<Transform3D>())
        {
            DN_CORE_WARN("Entity not valid, or does not have Transform3D!");
            return Vector3Zero();
        }
        return GetGlobalScale(entity_);
    }

    void SetGlobalRotation(Quaternion rotation)
    {
        if (!entity_.is_valid() || !entity_.has<Transform3D>())
        {
            DN_CORE_WARN("Entity not valid, or does not have Transform3D!");
            return;
        }
        SetGlobalRotation(entity_, rotation);
    }

    Quaternion GetGlobalRotation() const
    {
        if (!entity_.is_valid() || !entity_.has<Transform3D>())
        {
            DN_CORE_WARN("Entity not valid, or does not have Transform3D!");
            return QuaternionIdentity();
        }
        return GetGlobalRotation(entity_);
    }

    struct Transform3DImpl
    {
        Vector3 pos;
        Vector3 scale;
        Quaternion rot;
    };
    using ReflectionType = Transform3DImpl;
    Transform3D(const ReflectionType &impl) : position_(impl.pos), scale_(impl.scale), rotation_(impl.rot)
    {
    }
    ReflectionType reflection() const
    {
        return Transform3DImpl{position_, scale_, rotation_};
    }

  private:
    Vector3 position_;
    Vector3 scale_;
    Quaternion rotation_;
    Vector3 globalPositionCache;
    Vector3 globalScaleCache;
    Quaternion globalRotationCache;
    bool globalPositionCacheDirtyFlag = true;
    bool globalScaleCacheDirtyFlag = true;
    bool globalRotationCacheDirtyFlag = true;
    flecs::entity entity_ = flecs::entity::null();

    static void InvalidateCacheFlags(flecs::entity e)
    {
        if (!e.is_valid() || !e.has<Transform3D>())
        {
            DN_CORE_WARN("STATIC Entity not valid, or does not have Transform3D!");
            return;
        }
        Transform3D *tx = e.try_get_mut<Transform3D>();
        if (!tx)
        {
            return;
        }

        tx->InvalidateCacheFlags();
    }

    void PropagateInvalidateCacheFlags()
    {
        if (!entity_.is_valid() || !entity_.has<Transform3D>())
        {
            DN_CORE_WARN("STATIC Entity not valid, or does not have Transform3D!");
            return;
        }
        entity_.children([](flecs::entity child) {
            if (!child.is_valid() || !child.has<Transform3D>())
            {
                DN_CORE_WARN("Child not valid, or does not have Transform3D!");
                return;
            }
            InvalidateCacheFlags(child);
        });
    }

    void UpdateGlobalPositionCache(Vector3 newGlobalPosition)
    {
        globalPositionCache = newGlobalPosition;
        globalPositionCacheDirtyFlag = false;
    }

    void UpdateGlobalScaleCache(Vector3 newGlobalScale)
    {
        globalScaleCache = newGlobalScale;
        globalScaleCacheDirtyFlag = false;
    }

    void UpdateGlobalRotationCache(Quaternion newGlobalRotation)
    {
        globalRotationCache = newGlobalRotation;
        globalRotationCacheDirtyFlag = false;
    }

    static Vector3 GetGlobalPositionRecursively(flecs::world &world, flecs::entity e)
    {
        if (!e.is_valid())
        {
            return Vector3Zero();
        }
        const Transform3D *tx = e.try_get<Transform3D>();
        if (!tx)
        {
            return Vector3Zero();
        }
        if (!e.parent().is_valid())
        {
            return tx->GetPosition();
        }

        return Vector3Add(tx->GetPosition(), GetGlobalPositionRecursively(world, e.parent()));
    }

    static Vector3 GetGlobalScaleRecursively(flecs::world &world, flecs::entity e)
    {
        if (!e.is_valid())
        {
            return Vector3One();
        }
        const Transform3D *tx = e.try_get<Transform3D>();
        if (!tx)
        {
            return Vector3One();
        }
        if (!e.parent().is_valid())
        {
            return tx->GetScale();
        }

        return Vector3Multiply(tx->GetScale(), GetGlobalScaleRecursively(world, e.parent()));
    }

    static Quaternion GetGlobalRotationRecursively(flecs::world &world, flecs::entity e)
    {
        if (!e.is_valid())
        {
            return QuaternionIdentity();
        }
        const Transform3D *tx = e.try_get<Transform3D>();
        if (!tx)
        {
            return QuaternionIdentity();
        }
        if (!e.parent().is_valid())
        {
            return tx->GetRotation();
        }

        return QuaternionAdd(tx->GetRotation(), GetGlobalRotationRecursively(world, e.parent()));
    }

    static Transform3D GetGlobalTransformRecursively(flecs::world &world, flecs::entity e)
    {
        if (!e.is_valid())
        {
            return Transform3D();
        }
        const Transform3D *tx = e.try_get<Transform3D>();
        if (!tx)
        {
            return Transform3D();
        }
        if (!e.parent().is_valid())
        {
            return *tx;
        }

        Transform3D parentTx = GetGlobalTransformRecursively(world, e.parent());
        Transform3D resultTx(Vector3Add(tx->GetPosition(), parentTx.GetPosition()),
                             Vector3Multiply(tx->GetScale(), parentTx.GetScale()),
                             QuaternionAdd(tx->GetRotation(), parentTx.GetRotation()));

        return resultTx;
    }
};

/** @} */

/**
 * @struct Velocity3D
 * @brief 3D velocity component (always in global coordinates).
 */
struct Velocity3D
{
    Vector3 value;

    Velocity3D() : value(Vector3Zero())
    {
    }

    Velocity3D(Vector3 value) : value(value)
    {
    }

    Velocity3D(float x, float y, float z) : value(x, y, z)
    {
    }

    struct Velocity3DImpl
    {
        Vector3 v;
    };
    using ReflectionType = Velocity3DImpl;
    Velocity3D(const ReflectionType &impl) : value(impl.v)
    {
    }
    const ReflectionType &reflection() const
    {
        return Velocity3DImpl{value};
    }
};

/**
 * @name Visual Components
 * @{
 */

/** @brief Cube geometry component. */
struct CubeComponent
{
    float width, height, length;
    // TODO REPLACE_RAYLIB
    // ::Color color;
};

/** @} */

/**
 * @name Physics Body Components
 * Wrappers for PhysX body types.
 * @{
 */

/** @brief Character controller body component. */
struct CharacterBodyComponent
{
    CharacterBodyDesc desc;
    std::shared_ptr<CharacterBody> body;

    CharacterBodyComponent() : body(nullptr)
    {
    }

    CharacterBodyComponent(std::shared_ptr<CharacterBody> cb) : body(cb)
    {
    }

    using ReflectionType = CharacterBodyDesc;
    // TODO should it create the body directly, or should the body be seen as a resource?
    CharacterBodyComponent(const ReflectionType &impl) : desc(impl), body(CharacterBody::Create(impl))
    {
    }
    ReflectionType reflection() const
    {
        return body ? body->GetDescriptor() : desc;
    }
};

/** @brief Static physics body component. */
struct StaticBodyComponent
{
    std::shared_ptr<StaticBody> body;

    StaticBodyComponent() : body(nullptr)
    {
    }

    StaticBodyComponent(std::shared_ptr<StaticBody> sb) : body(sb)
    {
    }

    // TODO
    struct StaticBodyComponentImpl
    {
        int e;
    };
    using ReflectionType = StaticBodyComponentImpl;
    StaticBodyComponent(const ReflectionType &impl) : body(nullptr)
    {
    }
    const ReflectionType &reflection() const
    {
        return StaticBodyComponentImpl{0};
    }
};

/** @brief Kinematic physics body component. */
struct KinematicBodyComponent
{
    std::shared_ptr<KinematicBody> body;

    KinematicBodyComponent() : body(nullptr)
    {
    }

    KinematicBodyComponent(std::shared_ptr<KinematicBody> kb) : body(kb)
    {
    }

    // TODO
    struct KinematicBodyComponentImpl
    {
        int e;
    };
    using ReflectionType = KinematicBodyComponentImpl;
    KinematicBodyComponent(const ReflectionType &impl) : body(nullptr)
    {
    }
    const ReflectionType &reflection() const
    {
        return KinematicBodyComponentImpl{0};
    }
};

/** @brief Dynamic (fully simulated) physics body component. */
struct DynamicBodyComponent
{
    std::shared_ptr<DynamicBody> body;

    DynamicBodyComponent() : body(nullptr)
    {
    }

    DynamicBodyComponent(std::shared_ptr<DynamicBody> db) : body(db)
    {
    }

    // TODO
    struct DynamicBodyComponentImpl
    {
        int e;
    };
    using ReflectionType = DynamicBodyComponentImpl;
    DynamicBodyComponent(const ReflectionType &impl) : body(nullptr)
    {
    }
    const ReflectionType &reflection() const
    {
        return DynamicBodyComponentImpl{0};
    }
};

/** @brief Static cube physics body (stub). */
struct PhysicsStaticCubeComponent
{
    std::shared_ptr<PhysicsStaticCubeComponent> cube;
};
/** @} */

/**
 * @name Debug Components
 * Visual debugging aids.
 * @{
 */

/** @brief Debug capsule visualization. */
struct DebugCapsuleComponent
{
    float height, radius;
    int slices, rings;
    // TODO REPLACE_RAYLIB
    // ::Color color;
};

/** @brief Debug cube visualization. */
struct DebugCubeComponent
{
    float width, height, length;
};
/** @} */
}; // namespace ECSComponent

/**
 * @namespace ECSPrefab
 * @brief Entity prefab templates.
 * @ingroup ECS_Prefabs
 *
 * Prefabs are pre-configured entity templates. Create instances via
 * `world.entity().is_a(ECSPrefab::Node3D)`.
 */
namespace ECSPrefab
{
extern duin::Entity Node;   ///< Base node prefab.
extern duin::Entity Node2D; ///< 2D node with Position2D, Rotation2D, Scale2D.
extern duin::Entity Node3D; ///< 3D node with Transform3D.

extern duin::Entity PhysicsStaticBody;    ///< Static body prefab.
extern duin::Entity PhysicsKinematicBody; ///< Kinematic body prefab.
extern duin::Entity PhysicsDynamicBody;   ///< Dynamic body prefab.
extern duin::Entity PhysicsCharacterBody; ///< Character body prefab.

extern duin::Entity Camera3D; ///< 3D camera prefab.
extern duin::Entity Cube;     ///< Renderable cube prefab.

extern duin::Entity DebugCapsule; ///< Debug capsule prefab.
}; // namespace ECSPrefab

class JSONMember;

/**
 * @class GameWorld
 * @brief Central ECS world manager.
 * @ingroup ECS_Manager
 *
 * Owns the FLECS world and provides methods for entity creation,
 * component registration, and query execution. Each scene/tab in
 * the editor has its own GameWorld instance.
 *
 * @code
 * GameWorld ecs;
 * ecs.Initialize();
 *
 * duin::Entity e = ecs.world.CreateEntity()
 *     .IsA(ECSPrefab::Node3D)
 *     .Set<ECSComponent::Transform3D>({position});
 * @endcode
 */
class GameWorld : public World
{
  public:
    GameWorld();
    ~GameWorld();

    /** @brief Initializes the ECS world and registers components. */
    void Initialize(bool connectSignals = true);

    /** @brief Creates entity from JSON data. @deprecated Use SceneBuilder. */
    duin::Entity CreateEntityFromJSON(JSONMember &member);

    /** @brief Sets the given entity as the active camera. */
    void ActivateCameraEntity(duin::Entity entity);

    /** @brief Runs post-update queries. Called by engine. */
    void PostUpdateQueryExecution(double delta);
    /** @brief Runs post-physics queries. Called by engine. */
    void PostPhysicsUpdateQueryExecution(double delta);
    /** @brief Runs post-draw queries. Called by engine. */
    void PostDrawQueryExecution();
    /** @brief Runs post-draw-UI queries. Called by engine. */
    void PostDrawUIQueryExecution();

    /** @brief Clears all entities from the world. */
    void Clear();

    /**
     * @name Transform Queries
     * @{
     */
    void ExecuteQueryTransform3DHierarchicalUpdate();
    void ExecuteQueryCharacterBody3DUpdateTransform();
    void ExecuteQuerySyncDynamicBody3DTransform();
    /** @} */

    /** @brief Processes camera activation requests. */
    void ExecuteQuerySetCameraAsActive();

    /**
     * @name Render Queries
     * @{
     */
    void ExecuteQueryControlCamera();
    void ExecuteQueryDrawCube();
    void ExecuteQueryDrawDebugCapsule();
    void ExecuteQueryDrawDebugCube();
    /** @} */

  private:
    void InitializeRemoteExplorer();
};

} // namespace duin
