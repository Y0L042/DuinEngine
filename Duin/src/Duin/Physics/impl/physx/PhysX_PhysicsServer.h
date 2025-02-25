#pragma once

#include "Duin/Physics/PhysicsServer.h"

#include <PxPhysicsAPI.h>
#include <PxPhysics.h>
#include <characterkinematic/PxController.h>
#include <characterkinematic/PxControllerManager.h>
#include <cooking/PxCooking.h>

namespace duin {
    class PhysXPhysicsServer : virtual public PhysicsServer
    {
        public:
            static PhysXPhysicsServer& GetPxServer();

            physx::PxDefaultAllocator pxAllocatorCallback;
            physx::PxDefaultErrorCallback pxErrorCallback;
            physx::PxDefaultCpuDispatcher* pxDispatcher = NULL;

            physx::PxTolerancesScale pxToleranceScale;
            physx::PxFoundation* pxFoundation = NULL;
            physx::PxPhysics* pxPhysics = NULL;

            physx::PxScene* pxScene = NULL;
            physx::PxControllerManager* pxControllerManager = NULL;
            physx::PxMaterial* pxMaterial = NULL;

            physx::PxPvd* pxPvd = NULL;

            PhysXPhysicsServer();
            ~PhysXPhysicsServer();

            void Initialize() override;
            void Clean() override;

            void StepPhysics(double delta) override;

            PhysXPhysicsServer(const PhysXPhysicsServer&) = delete;
            PhysXPhysicsServer& operator=(const PhysXPhysicsServer&) = delete;


        protected:

    };
}
