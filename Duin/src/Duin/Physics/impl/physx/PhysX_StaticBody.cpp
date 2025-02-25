#include "dnpch.h"
#include "PhysX_StaticBody.h"

namespace duin {
    PhysXStaticBody::PhysXStaticBody()
    {
        PhysXPhysicsServer& server = GetPxServer();
        pxMaterial = server.pxPhysics->createMaterial(0.5f, 0.5f, 0.6f);
        pxActor = physx::PxCreatePlane(*server.pxPhysics, physx::PxPlane(0, 1, 0, 0), *pxMaterial);
        server.pxScene->addActor(*pxActor);
    }

    PhysXStaticBody::~PhysXStaticBody()
    {
    }
}
