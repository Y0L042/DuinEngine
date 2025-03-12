#include "dnpch.h"
#include "DynamicBody.h"

#include "PhysicsServer.h"

#include "Duin/Core/Maths/DuinMaths.h"

#include <PxPhysicsAPI.h>

namespace duin {
    DynamicBody::DynamicBody(Transform3D transform, CollisionShape collisionShape)
        : collisionShape(collisionShape)
    {
        PhysicsServer& server = PhysicsServer::Get();
        physx::PxPhysics *pxPhysics = server.pxPhysics;

        actor = pxPhysics->createRigidDynamic(transform.ToPhysX());
        physx::PxShape *shape = collisionShape.pxShape;
        actor->attachShape(*shape);
        server.pxScene->addActor(*actor);
    }

    DynamicBody::~DynamicBody()
    {
        if (actor)
            actor->release();
        actor = nullptr;
    }
}
