#include "dnpch.h"
#include "StaticBody.h"

#include "PhysicsServer.h"

#include <PxPhysicsAPI.h>

namespace duin {
    StaticBody::StaticBody(Vector3 position, CollisionShape collisionShape)
        : collisionShape(collisionShape)
    {
        PhysicsServer& server = PhysicsServer::Get();
        physx::PxPhysics *pxPhysics = server.pxPhysics;
        actor = pxPhysics->createRigidStatic({ position.x, position.y, position.z });
        physx::PxShape *shape = collisionShape.pxShape;
        actor->attachShape(*shape);
        server.pxScene->addActor(*actor);
    }

    StaticBody::~StaticBody()
    {
        // if (actor)
        //     actor->release();
    }
}
