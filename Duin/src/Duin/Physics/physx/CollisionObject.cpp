#include "dnpch.h"
#include "CollisionObject.h"

#include "PhysicsServer.h"

namespace duin
{
void CollisionObject::SetCollisionShape()
{
    // TODO
}

const UUID CollisionObject::GetUUID()
{
    return uuid;
}

CollisionLayer CollisionObject::GetCollisionLayer()
{
    return collisionLayer;
}

CollisionMask CollisionObject::GetCollisionMask()
{
    return collisionMask;
}

void CollisionObject::SetCollisionLayer(CollisionLayer bits)
{
    collisionLayer = bits;
}

void CollisionObject::SetCollisionMask(CollisionMask bits)
{
    collisionMask = bits;
}

PhysicsServer &CollisionObject::GetPhysicsServer()
{
    return PhysicsServer::Get();
}
} // namespace duin
