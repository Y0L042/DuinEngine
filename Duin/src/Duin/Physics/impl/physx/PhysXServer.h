#pragma once

#include "../../PhysicsServer.h"

#include <PxPhysicsAPI.h>
#include <PxPhysics.h>
#include <characterkinematic/PxController.h>
#include <characterkinematic/PxControllerManager.h>
#include <cooking/PxCooking.h>

namespace duin {
    class PhysXServer : public PhysicsServer
    {
        public:
            PhysXServer();
            ~PhysXServer() override;

            int Initialize() override;
            void Clean() override;

            void StepPhysics(double delta) override;

            PhysXServer(const PhysXServer&) = delete;
            PhysXServer& operator=(const PhysXServer&) = delete;

        private:
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

    };
}
