/**
 * @file ECSComponents.h
 * @brief Data components for entities (spatial transforms, velocity).
 * @ingroup ECS_Components
 */

#pragma once

#include <flecs.h>
#include <rfl.hpp>
#include "Duin/ECS/DECS/World.h"
#include "Duin/Core/Maths/DuinMaths.h"
#include "Duin/Core/Debug/DNLog.h"

namespace duin
{

class GameWorld;

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
    Transform3D(Vector3 position = Vector3Zero(), Vector3 scale = Vector3One(),
                Quaternion rotation = QuaternionIdentity())
        : position_(position), scale_(scale), rotation_(rotation) {};

    Transform3D(Vector3 position, Quaternion rotation)
        : position_(position), scale_(Vector3One()), rotation_(rotation) {};

    Transform3D(const Transform3D &tx)
        : position_(tx.GetPosition()), scale_(tx.GetScale()), rotation_(tx.GetRotation()) {};

    void InvalidateCacheFlags()
    {
        globalPositionCacheDirtyFlag = true;
        globalScaleCacheDirtyFlag = true;
        globalRotationCacheDirtyFlag = true;
    }

    Vector3 SetPosition(Vector3 position)
    {
        position_ = position;
        InvalidateCacheFlags();

        return position_;
    }

    Vector3 GetPosition() const
    {
        return position_;
    }

    Vector3 SetScale(Vector3 scale)
    {
        scale_ = scale;
        InvalidateCacheFlags();

        return scale_;
    }

    Vector3 GetScale() const
    {
        return scale_;
    }

    Quaternion SetRotation(Quaternion rotation)
    {
        rotation_ = rotation;
        InvalidateCacheFlags();

        return rotation_;
    }

    Quaternion GetRotation() const
    {
        return rotation_;
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
    friend class duin::GameWorld;

    Vector3 position_;
    Vector3 scale_;
    Quaternion rotation_;
    Vector3 globalPositionCache;
    Vector3 globalScaleCache;
    Quaternion globalRotationCache;
    bool globalPositionCacheDirtyFlag = true;
    bool globalScaleCacheDirtyFlag = true;
    bool globalRotationCacheDirtyFlag = true;

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

void RegisterComponents(duin::World &world);

}; // namespace ECSComponent

} // namespace duin
