#include "dnpch.h"
#include "CollisionObject.h"

#include "PhysicsServer.h"

namespace duin {
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

    PhysicsServer& CollisionObject::GetServer()
    {
        return PhysicsServer::Get();
    }
}
