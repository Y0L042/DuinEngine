#include "dnpch.h"
#include "KinematicBody.h"

#include "PhysicsServer.h"

#include "Duin/Core/Maths/DuinMaths.h"

#include <PxPhysicsAPI.h>

namespace duin {
    std::shared_ptr<KinematicBody> KinematicBody::Create(Transform3D transform, CollisionShape collisionShape)
    {
        return std::make_shared<KinematicBody>(transform, collisionShape);
    }

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
        if (actor) {
            actor->release();
            actor = nullptr;
        }
    }
    Vector3 KinematicBody::GetPosition()
    {
        return Vector3();
    }
}
