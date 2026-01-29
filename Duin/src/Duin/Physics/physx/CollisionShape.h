/**
 * @file CollisionShape.h
 * @brief Collision shape combining geometry and material.
 * @ingroup Physics_Collision
 */

#pragma once

#include <variant>

#include <PxPhysicsAPI.h>

#include "PhysicsMaterial.h"
#include "CollisionGeometry.h"

namespace duin
{

/**
 * @brief Variant type for collision geometry.
 * @ingroup Physics_Collision
 */
using CollisionGeometryVariant =
    std::variant<BoxGeometry, SphereGeometry, CapsuleGeometry, PlaneGeometry, ConvexMeshGeometry, TriangleMeshGeometry>;

/**
 * @class CollisionShape
 * @brief Combines geometry and material for physics collision.
 * @ingroup Physics_Collision
 *
 * Wraps a geometry variant and physics material into a shape
 * that can be attached to physics bodies.
 */
class CollisionShape
{
  public:
    /**
     * @brief Constructs a collision shape.
     * @param geometryVariant The geometry type (box, sphere, capsule, etc.).
     * @param physicsMaterial Material properties (friction, restitution).
     */
    CollisionShape(CollisionGeometryVariant geometryVariant, PhysicsMaterial physicsMaterial = PhysicsMaterial());
    ~CollisionShape();

    /** @brief Returns the geometry variant. */
    CollisionGeometryVariant &GetGeometryVariant();

  private:
    friend class StaticBody;
    friend class KinematicBody;
    friend class DynamicBody;

    CollisionGeometryVariant geometryVariant;
    PhysicsMaterial physicsMaterial;
    physx::PxShape *pxShape = nullptr;
};

} // namespace duin
