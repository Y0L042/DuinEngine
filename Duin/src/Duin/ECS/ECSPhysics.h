/**
 * @file ECSPhysics.h
 * @brief Physics body and debug/visual component structs.
 * @ingroup ECS_Components
 */

#pragma once

#include "ECSComponents.h"
#include "Duin/Physics/PhysicsIncludes.h"

namespace duin
{

namespace ECSComponent
{

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
    CharacterBodyComponent(const ReflectionType &impl) : desc(impl), body(CharacterBody::Create(impl, {0, 0, 0}))
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

} // namespace duin
