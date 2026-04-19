#include "dnpch.h"
#include "CollisionObject.h"
#include "PhysicsServer.h"

void duin::CollisionObject::SetCollisionShape()
{
}

const duin::UUID duin::CollisionObject::GetUUID()
{
    return uuid;
}

duin::CollisionLayer duin::CollisionObject::GetCollisionLayer()
{
    return collisionLayer;
}

duin::CollisionMask duin::CollisionObject::GetCollisionMask()
{
    return collisionMask;
}

void duin::CollisionObject::SetCollisionLayer(duin::CollisionLayer bits)
{
    collisionLayer = bits;
}

void duin::CollisionObject::SetCollisionMask(duin::CollisionMask bits)
{
    collisionMask = bits;
}

duin::PhysicsServer &duin::CollisionObject::GetPhysicsServer()
{
    return duin::PhysicsServer::Get();
}
