#include "dnpch.h"
#include "PhysicsStaticPlane3D.h"
#include "Physics3DServer.h"

namespace duin {
    StaticCollisionPlane::StaticCollisionPlane(Physics3DServer& server)
        : server(server)
    {
        physx::PxMaterial *pxMaterial = server.pxPhysics->createMaterial(0.5f, 0.5f, 0.6f);
        physx::PxRigidStatic *groundPlane = physx::PxCreatePlane(*server.pxPhysics, physx::PxPlane(0, 1, 0, 0), *pxMaterial);
        server.pxScene->addActor(*groundPlane);
    }
}
