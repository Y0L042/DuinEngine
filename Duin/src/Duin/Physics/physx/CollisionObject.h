/**
 * @file CollisionObject.h
 * @brief Base interface for collision-enabled physics objects.
 * @ingroup Physics_Collision
 */

#pragma once

#include "Duin/Core/Utils/UUID.h"
#include "CollisionMeta.h"

#include <memory>

namespace duin
{

class PhysicsServer;

/**
 * @class CollisionObject
 * @brief Base class for objects participating in collision detection.
 * @ingroup Physics_Collision
 *
 * Provides collision layer/mask filtering. Layers define what an object
 * is; masks define what it collides with.
 */
class CollisionObject
{
  public:
    void SetCollisionShape();

    /** @brief Returns this object's unique identifier. */
    const UUID GetUUID();

    /** @brief Returns the collision layer bits. */
    CollisionLayer GetCollisionLayer();
    /** @brief Returns the collision mask bits. */
    CollisionMask GetCollisionMask();
    /** @brief Sets which layer this object belongs to. */
    void SetCollisionLayer(CollisionLayer bits);
    /** @brief Sets which layers this object can collide with. */
    void SetCollisionMask(CollisionMask bits);

    /** @brief Returns reference to the physics server. */
    PhysicsServer &GetPhysicsServer();

  private:
    UUID uuid;
    CollisionLayer collisionLayer;
    CollisionMask collisionMask;
};

} // namespace duin
