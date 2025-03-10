#pragma once
#include "Duin/Physics/PhysicsServer.h"

#include <memory>
#include <unordered_map>

#include "Duin/Core/Utils/UUID.h"

#include <PxPhysicsAPI.h>
#include <PxPhysics.h>
#include <characterkinematic/PxController.h>
#include <characterkinematic/PxControllerManager.h>
#include <cooking/PxCooking.h>

namespace duin {
    class PhysXCollisionShape;
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
            std::shared_ptr<CollisionShape> CreateCollisionShape() override;

            std::shared_ptr<PhysXCollisionShape> GetPhysXCollisionShape(UUID uuid);

            PhysXPhysicsServer(const PhysXPhysicsServer&) = delete;
            PhysXPhysicsServer& operator=(const PhysXPhysicsServer&) = delete;

        protected:
            std::unordered_map<UUID, std::shared_ptr<PhysXCollisionShape>> shapesMap;

    };
}
