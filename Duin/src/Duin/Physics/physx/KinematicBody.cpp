#include "dnpch.h"
#include "KinematicBody.h"

#include "PhysicsServer.h"

#include "Duin/Core/Maths/DuinMaths.h"

#include <PxPhysicsAPI.h>

namespace duin {
    KinematicBody::KinematicBody(Transform3D transform, CollisionShape collisionShape)
        : collisionShape(collisionShape)
    {
        PhysicsServer& server = PhysicsServer::Get();
        physx::PxPhysics *pxPhysics = server.pxPhysics;

        actor = pxPhysics->createRigidDynamic(transform.ToPhysX());
        actor->setRigidBodyFlag(physx::PxRigidBodyFlag::eKINEMATIC, true);
        physx::PxShape *shape = collisionShape.pxShape;
        actor->attachShape(*shape);
        server.pxScene->addActor(*actor);
    }

    KinematicBody::~KinematicBody()
    {
        // if (actor)
        //     actor->release();
    }
}
