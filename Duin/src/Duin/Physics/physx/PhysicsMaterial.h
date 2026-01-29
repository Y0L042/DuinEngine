/**
 * @file PhysicsMaterial.h
 * @brief Physical material properties for collision.
 * @ingroup Physics_Collision
 */

#pragma once

#include <PxPhysicsAPI.h>

namespace duin
{

/**
 * @class PhysicsMaterial
 * @brief Defines surface properties for physics simulation.
 * @ingroup Physics_Collision
 *
 * Controls friction and bounciness of physics bodies.
 */
class PhysicsMaterial
{
  public:
    float staticFriction;   ///< Friction when stationary (0.0 - 1.0+).
    float dynamicFriction;  ///< Friction when moving (0.0 - 1.0+).
    float restitutionCoeff; ///< Bounciness (0.0 = no bounce, 1.0 = full bounce).

    /**
     * @brief Constructs a physics material.
     * @param staticFriction Friction coefficient when object is at rest.
     * @param dynamicFriction Friction coefficient when object is moving.
     * @param restitutionCoeff Coefficient of restitution (bounciness).
     */
    PhysicsMaterial(float staticFriction = 1.0f, float dynamicFriction = 1.0f, float restitutionCoeff = 1.0f);

    physx::PxMaterial *pxMaterial = nullptr; ///< Underlying PhysX material.

  private:
    class CollisionShape;
    friend class CollisionShape;
    friend class PhysicsMaterial;
};

} // namespace duin
