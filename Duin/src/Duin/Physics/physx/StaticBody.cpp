#include "dnpch.h"
#include "StaticBody.h"

#include "PhysicsServer.h"

#include "Duin/Core/Maths/DuinMaths.h"

#include <PxPhysicsAPI.h>

namespace duin {
    std::shared_ptr<StaticBody> StaticBody::Create(Transform3D transform, CollisionShape collisionShape)
    {
        return std::make_shared<StaticBody>(transform, collisionShape);
    }

    StaticBody::StaticBody(Transform3D transform, CollisionShape collisionShape)
        : collisionShape(collisionShape)
    {
        PhysicsServer& server = PhysicsServer::Get();
        physx::PxPhysics *pxPhysics = server.pxPhysics;

        actor = pxPhysics->createRigidStatic(transform.ToPhysX());
        physx::PxShape *shape = collisionShape.pxShape;
        actor->attachShape(*shape);
        server.pxScene->addActor(*actor);
    }

    StaticBody::~StaticBody()
    {
        if (actor) {
            actor->release();
            actor = nullptr;
        }
    }
    Vector3 StaticBody::GetPosition()
    {
        return Vector3();
    }
}
