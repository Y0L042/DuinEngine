#include "dnpch.h"
#include "DynamicBody.h"

#include "PhysicsServer.h"

#include "Duin/Core/Maths/DuinMaths.h"

#include <PxPhysicsAPI.h>

namespace duin {
    std::shared_ptr<DynamicBody> DynamicBody::Create(Transform3D transform, CollisionShape collisionShape)
    {
        return std::make_shared<DynamicBody>(transform, collisionShape);
    }

    std::shared_ptr<DynamicBody> DynamicBody::Create(Transform3D transform, CollisionGeometryVariant geometry, PhysicsMaterial material)
    {
        return std::make_shared<DynamicBody>(transform, geometry, material);
    }

    DynamicBody::DynamicBody(Transform3D transform, CollisionShape collisionShape)
        : collisionShape(collisionShape)
    {
        PhysicsServer& server = PhysicsServer::Get();
        physx::PxPhysics *pxPhysics = server.pxPhysics;

        actor = pxPhysics->createRigidDynamic(transform.ToPhysX());
        actor->setRigidBodyFlag(physx::PxRigidBodyFlag::eKINEMATIC, false);
        physx::PxShape *shape = collisionShape.pxShape;
        actor->attachShape(*shape);
        server.pxScene->addActor(*actor);
    }

    DynamicBody::DynamicBody(Transform3D transform, CollisionGeometryVariant geometry, PhysicsMaterial material)
        : collisionShape(CollisionShape(geometry, material))
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
        // TODO
        //if (actor) {
        //    actor->release();
        //    actor = nullptr;
        //}
    }
    Vector3 DynamicBody::GetPosition()
    {
        return Vector3(actor->getGlobalPose().p);
    }

    Quaternion DynamicBody::GetRotation()
    {
        return Quaternion(actor->getGlobalPose().q);
    }
}
