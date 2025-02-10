#include "dnpch.h"
#include "PhysicsStaticBody3D.h"
#include "Physics3DServer.h"

namespace duin {
    PhysicsStaticPlane3D::PhysicsStaticPlane3D(Physics3DServer& server)
        : server(server)
    {
        physx::PxMaterial *pxMaterial = server.pxPhysics->createMaterial(0.5f, 0.5f, 0.6f);
        physx::PxRigidStatic *groundPlane = physx::PxCreatePlane(*server.pxPhysics, physx::PxPlane(0, 1, 0, 0), *pxMaterial);
        server.pxScene->addActor(*groundPlane);
    }


    PhysicsStaticCube3D::PhysicsStaticCube3D(Physics3DServer& server, Vector3 position, Vector3 size)
        : server(server)
    {
        physx::PxMaterial *pxMaterial = server.pxPhysics->createMaterial(0.5f, 0.5f, 0.6f);

        physx::PxBoxGeometry boxGeometry(size.x * 0.5f, size.y * 0.5f, size.z * 0.5f);
        physx::PxShape* shape = server.pxPhysics->createShape(boxGeometry, *pxMaterial);
        physx::PxTransform transform = physx::PxTransform(physx::PxVec3(position.x, position.y, position.z));
        physx::PxRigidStatic* body = server.pxPhysics->createRigidStatic(transform);
        body->attachShape(*shape);
        shape->release();
        server.pxScene->addActor(*body);
    }
}
/*
 PxRigidActorExt::createExclusiveShape(*aBoxActor,
    PxBoxGeometry(a/2, b/2, c/2), aMaterial);
*/

/*
static void createStack(const PxTransform& t, PxU32 size, PxReal halfExtent)
{
	PxShape* shape = gPhysics->createShape(PxBoxGeometry(halfExtent, halfExtent, halfExtent), *gMaterial);
	for(PxU32 i=0; i<size;i++)
	{
		for(PxU32 j=0;j<size-i;j++)
		{
			PxTransform localTm(PxVec3(PxReal(j*2) - PxReal(size-i), PxReal(i*2+1), 0) * halfExtent);
			PxRigidDynamic* body = gPhysics->createRigidDynamic(t.transform(localTm));
			body->attachShape(*shape);
			PxRigidBodyExt::updateMassAndInertia(*body, 10.0f);
			gScene->addActor(*body);
		}
	}
	shape->release();
}
*/
