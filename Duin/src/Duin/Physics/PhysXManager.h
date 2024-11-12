#pragma once

#include <PxPhysicsAPI.h>
#include <PxPhysics.h>
#include <characterkinematic/PxController.h>
#include <characterkinematic/PxControllerManager.h>

namespace duin {

class PhysXManager
{
public:
	PhysXManager();
	~PhysXManager();

	void Initialize();
	void InitializePVDClient();
	void Clean();

	void StepPhysics(double delta);

private:
	physx::PxDefaultAllocator defaultAllocatorCallback;
	physx::PxDefaultErrorCallback defaultErrorCallback;
	physx::PxDefaultCpuDispatcher* pxDispatcher = NULL;

	physx::PxTolerancesScale pxToleranceScale;
	physx::PxFoundation* pxFoundation = NULL;
	physx::PxPhysics* pxPhysics = NULL;

	physx::PxScene* pxScene = NULL;
	physx::PxControllerManager* pxControllerManager = NULL;
	physx::PxMaterial* pxMaterial = NULL;

	physx::PxPvd* pxPvd = NULL;
};

}
