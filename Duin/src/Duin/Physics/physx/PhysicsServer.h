#pragma once

#include <PxPhysicsAPI.h>
#include <PxPhysics.h>
#include <characterkinematic/PxController.h>
#include <characterkinematic/PxControllerManager.h>
#include <cooking/PxCooking.h>

namespace duin {
    enum PhysicsServerError {
        SUCCESS = 0,
        INIT_FAILED,
        MATERIAL_CREATION_FAILED
    };

    class UUID;
    class CollisionObject;
    class CollisionShape;
    class CharacterBody;
    class PhysicsServer
    {
        public:
            static PhysicsServer& Get();

            void Initialize();
            void Clean();

            void StepPhysics(double delta);

       protected:
            friend class StaticBody;
            friend class KinematicBody;
            friend class DynamicBody;
            friend class CharacterBody;
            friend class CollisionShape;
            friend class PhysicsMaterial;
            
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

            PhysicsServer();
            ~PhysicsServer();
    };
}

#undef PHYSICS_SERVER
