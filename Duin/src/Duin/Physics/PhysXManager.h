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

    class PhysXManager;
    class CharacterBody3D
    {
    public:
        PhysXManager& manager;
        physx::PxCapsuleControllerDesc desc;
        physx::PxController *controller = NULL;
        physx::PxControllerFilters filters;
        physx::PxObstacleContext *obstacles = NULL;

        CharacterBody3D(PhysXManager& manager, CharacterBody3DDesc desc);
        ~CharacterBody3D();

        void Move(Vector3 displacement, double delta);

        CharacterBody3D(const CharacterBody3D&) = delete;
        CharacterBody3D& operator=(const CharacterBody3D&) = delete;

    private:
    };
    
    struct StaticCollisionPlaneDesc
    {
        
    };

    class PhysXManager;
    class StaticCollisionPlane
    {
    public:
        PhysXManager& manager;

        StaticCollisionPlane(PhysXManager& manager);

    private:

    };

    class PhysXManager
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

        PhysXManager();
        ~PhysXManager();

        void Initialize();
        void Clean();

        void StepPhysics(double delta);

        PhysXManager(const PhysXManager&) = delete;
        PhysXManager& operator=(const PhysXManager&) = delete;

    private:

    };

}
