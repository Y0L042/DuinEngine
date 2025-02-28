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

    PhysXStaticBody::PhysXStaticBody(PhysicsMaterial material)
    {
        PhysXPhysicsServer& server = GetPxServer();
        pxMaterial = server.pxPhysics->createMaterial( material.staticFriction, 
                                                      material.dynamicFriction, 
                                                      material.restitutionCoeff);
        pxActor = physx::PxCreatePlane(*server.pxPhysics, physx::PxPlane(0, 1, 0, 0), *pxMaterial);
        server.pxScene->addActor(*pxActor);
    }

    PhysXStaticBody::~PhysXStaticBody()
    {
    }

    void PhysXStaticBody::SetCollisionShape()
    {

    }
}
