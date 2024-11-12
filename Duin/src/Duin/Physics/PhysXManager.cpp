#include "dnpch.h"
#include "PhysXManager.h"

#include "Duin/Core/Debug/DNLog.h"

namespace duin {

PhysXManager::PhysXManager()
{
	Initialize();
}

PhysXManager::~PhysXManager()
{
}

void PhysXManager::Initialize()
{
	pxFoundation = PxCreateFoundation(PX_PHYSICS_VERSION,
		defaultAllocatorCallback,
		defaultErrorCallback);

	pxPvd = physx::PxCreatePvd(*pxFoundation);
	physx::PxPvdTransport* transport = physx::PxDefaultPvdSocketTransportCreate("127.0.0.1", 5425, 10);
	pxPvd->connect(*transport, physx::PxPvdInstrumentationFlag::eALL);

	pxPhysics = PxCreatePhysics(PX_PHYSICS_VERSION, *pxFoundation, physx::PxTolerancesScale(), true, pxPvd);

	physx::PxSceneDesc sceneDesc(pxPhysics->getTolerancesScale());
	sceneDesc.gravity = physx::PxVec3(0.0f, -9.81f, 0.0f);
	pxDispatcher = physx::PxDefaultCpuDispatcherCreate(2);
	sceneDesc.cpuDispatcher = pxDispatcher;
	sceneDesc.filterShader = physx::PxDefaultSimulationFilterShader;
	pxScene = pxPhysics->createScene(sceneDesc);

	physx::PxPvdSceneClient* pvdClient = pxScene->getScenePvdClient();
	if (pvdClient)
	{
		pvdClient->setScenePvdFlag(physx::PxPvdSceneFlag::eTRANSMIT_CONSTRAINTS, true);
		pvdClient->setScenePvdFlag(physx::PxPvdSceneFlag::eTRANSMIT_CONTACTS, true);
		pvdClient->setScenePvdFlag(physx::PxPvdSceneFlag::eTRANSMIT_SCENEQUERIES, true);
	}

	pxMaterial = pxPhysics->createMaterial(0.5f, 0.5f, 0.6f);

	physx::PxRigidStatic* groundPlane = physx::PxCreatePlane(*pxPhysics, physx::PxPlane(0, 1, 0, 0), *pxMaterial);
	pxScene->addActor(*groundPlane);

}

void PhysXManager::InitializePVDClient()
{
	if (!pxScene) {
		DN_CORE_WARN("PxScene is NULL!");
		return;
	}
	physx::PxPvdSceneClient* pvdClient = pxScene->getScenePvdClient();
	if (pvdClient)
	{
		pvdClient->setScenePvdFlag(physx::PxPvdSceneFlag::eTRANSMIT_CONSTRAINTS, true);
		pvdClient->setScenePvdFlag(physx::PxPvdSceneFlag::eTRANSMIT_CONTACTS, true);
		pvdClient->setScenePvdFlag(physx::PxPvdSceneFlag::eTRANSMIT_SCENEQUERIES, true);
	}
	else {
		DN_CORE_WARN("PxPvdClient not created!");
		return;
	}
}

void PhysXManager::Clean()
{
	PX_RELEASE(pxScene);
	PX_RELEASE(pxDispatcher);
	PX_RELEASE(pxPhysics);
	if (pxPvd)
	{
		physx::PxPvdTransport* transport = pxPvd->getTransport();
		PX_RELEASE(pxPvd);
		PX_RELEASE(transport);
	}
	PX_RELEASE(pxFoundation);
}

void PhysXManager::StepPhysics(double delta)
{
	pxScene->simulate(delta);
	pxScene->fetchResults(true);
}

}