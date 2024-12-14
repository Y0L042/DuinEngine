#pragma once

#include <PxPhysicsAPI.h>
#include <PxPhysics.h>
#include <characterkinematic/PxController.h>
#include <characterkinematic/PxControllerManager.h>
#include <cooking/PxCooking.h>

#include "Duin/Core/Maths/DuinMaths.h"

#include <vector>

namespace duin {

    struct PhysicsMaterial 
    {
        float staticFriction;
        float dynamicFriction;
        float restitutionCoeff;
    };


    


    struct StaticCollisionPlaneDesc
    {
        
    };

    class Physics3DServer;
    class StaticCollisionPlane
    {
    public:
        Physics3DServer& server;

        StaticCollisionPlane(Physics3DServer& server);

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
        physx::PxMaterial* CreateMaterial(float staticFriction, float dynamicFriction, float restitution);
        physx::PxMaterial* CreateMaterial(PhysicsMaterial material);

        void StepPhysics(double delta);

        Physics3DServer(const Physics3DServer&) = delete;
        Physics3DServer& operator=(const Physics3DServer&) = delete;

    private:

    };

}
