/**
 * @file DynamicBody.h
 * @brief Fully simulated rigid body.
 * @ingroup Physics_Bodies
 */

#pragma once

#include "PhysicsObject.h"
#include "CollisionShape.h"
#include "PhysicsStructs.h"

#include "Duin/Core/Maths/DuinMaths.h"

#include <memory>

#include <PxPhysicsAPI.h>

namespace duin
{

/**
 * @class DynamicBody
 * @brief A fully physics-simulated rigid body.
 * @ingroup Physics_Bodies
 *
 * Dynamic bodies respond to forces, gravity, and collisions. Their
 * position and rotation are controlled by the physics simulation.
 *
 * Use Create() factory methods for proper initialization.
 */
class DynamicBody : public PhysicsObject
{
  public:
    /** @brief Creates a dynamic body with a collision shape. */
    static std::shared_ptr<DynamicBody> Create(Transform3D transform, CollisionShape collisionShape);
    /** @brief Creates a dynamic body with geometry and material. */
    static std::shared_ptr<DynamicBody> Create(Transform3D transform, CollisionGeometryVariant geometry,
                                               PhysicsMaterial material);

    DynamicBody(Transform3D transform, CollisionShape collisionShape);
    DynamicBody(Transform3D transform, CollisionGeometryVariant geometry, PhysicsMaterial material);
    ~DynamicBody();

    /** @brief Returns current world position. */
    Vector3 GetPosition() override;
    /** @brief Returns current world rotation. */
    Quaternion GetRotation();

  private:
    CollisionShape collisionShape;
    physx::PxRigidDynamic *actor = nullptr;
};

} // namespace duin
