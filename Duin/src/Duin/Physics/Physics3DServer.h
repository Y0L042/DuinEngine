#pragma once

#include <PxPhysicsAPI.h>
#include <PxPhysics.h>
#include <characterkinematic/PxController.h>
#include <characterkinematic/PxControllerManager.h>

#include "Duin/Core/Maths/DuinMaths.h"

namespace duin {

    struct PhysicsMaterial 
    {
        float staticFriction;
        float dynamicFriction;
        float restitutionCoeff;
    };

    struct CharacterBody3DDesc 
    {
        float height;
        float radius;
        float slopeLimit;
        float stepOffset;
        float contactOffset;
        Vector3 position;
        Vector3 upDirection;
        physx::PxMaterial *material;
        void *reportCallback;
        void *behaviourCallback;

        physx::PxCapsuleControllerDesc ToPhysX()
        {
            physx::PxCapsuleControllerDesc desc;

            desc.height = height;
            desc.radius = radius;
            desc.slopeLimit = slopeLimit;
            desc.stepOffset = stepOffset;
            desc.contactOffset = contactOffset;
            desc.position = position.ToPhysXd();
            desc.material = material;
            desc.upDirection = upDirection.ToPhysX();
            desc.reportCallback = (physx::PxUserControllerHitReport *)reportCallback;
            desc.behaviorCallback = (physx::PxControllerBehaviorCallback *)behaviourCallback;

            return desc;
        }
    };

    class Physics3DServer;
    class CharacterBody3D
    {
    public:
        Physics3DServer& manager;
        physx::PxCapsuleControllerDesc desc;
        physx::PxController *controller = NULL;
        physx::PxControllerFilters filters;
        physx::PxObstacleContext *obstacles = NULL;

        CharacterBody3D(Physics3DServer& manager, CharacterBody3DDesc desc);
        ~CharacterBody3D();

        void Move(Vector3 displacement, double delta);

        CharacterBody3D(const CharacterBody3D&) = delete;
        CharacterBody3D& operator=(const CharacterBody3D&) = delete;

    private:
    };
    
    struct StaticCollisionPlaneDesc
    {
        
    };

    class Physics3DServer;
    class StaticCollisionPlane
    {
    public:
        Physics3DServer& manager;

        StaticCollisionPlane(Physics3DServer& manager);

    private:

    };

    class Physics3DServer
    {
    public:
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

        Physics3DServer();
        ~Physics3DServer();

        void Initialize();
        void Clean();

        void StepPhysics(double delta);

        Physics3DServer(const Physics3DServer&) = delete;
        Physics3DServer& operator=(const Physics3DServer&) = delete;

    private:

    };

}
