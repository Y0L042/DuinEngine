#include "dnpch.h"
#include "Physics3DServer.h"

#include "Duin/Core/Debug/DNLog.h"
#include "Duin/Core/Application.h"







namespace duin {
    StaticCollisionPlane::StaticCollisionPlane(Physics3DServer& server)
        : server(server)
    {
        physx::PxMaterial *pxMaterial = server.pxPhysics->createMaterial(0.5f, 0.5f, 0.6f);
        physx::PxRigidStatic *groundPlane = physx::PxCreatePlane(*server.pxPhysics, physx::PxPlane(0, 1, 0, 0), *pxMaterial);
        server.pxScene->addActor(*groundPlane);
    }
}





namespace duin {
    Physics3DServer::Physics3DServer()
    {
        Initialize();
    }

    Physics3DServer::~Physics3DServer()
    {
    }

    void Physics3DServer::Initialize()
    {
        pxFoundation = PxCreateFoundation(PX_PHYSICS_VERSION,
            pxAllocatorCallback,
            pxErrorCallback);

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

        pxMaterial = pxPhysics->createMaterial(0.5f, 0.5f, 0.6f);

        physx::PxPvdSceneClient* pvdClient = pxScene->getScenePvdClient();
        if (pvdClient)
        {
            pvdClient->setScenePvdFlag(physx::PxPvdSceneFlag::eTRANSMIT_CONSTRAINTS, true);
            pvdClient->setScenePvdFlag(physx::PxPvdSceneFlag::eTRANSMIT_CONTACTS, true);
            pvdClient->setScenePvdFlag(physx::PxPvdSceneFlag::eTRANSMIT_SCENEQUERIES, true);
        }

        pxControllerManager = PxCreateControllerManager(*pxScene);
    }

    void Physics3DServer::Clean()
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

    physx::PxMaterial* Physics3DServer::CreateMaterial(float staticFriction, float dynamicFriction, float restitution)
    {
        return pxPhysics->createMaterial(staticFriction, dynamicFriction, restitution);
    }

    physx::PxMaterial* Physics3DServer::CreateMaterial(PhysicsMaterial material)
    {
        return pxPhysics->createMaterial(material.staticFriction, material.dynamicFriction, material.restitutionCoeff);
    }

    void Physics3DServer::StepPhysics(double delta)
    {
        pxScene->simulate((float)delta);
        pxScene->fetchResults(true);
    }
}
