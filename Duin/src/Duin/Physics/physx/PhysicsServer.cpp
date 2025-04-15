#include "dnpch.h"
#include "PhysicsServer.h"

#include "Duin/Core/Debug/DNLog.h"
#include "CollisionShape.h"

#define DEBUG

namespace duin {
    PhysicsServer& PhysicsServer::Get()
    {
        static PhysicsServer server;
        return server;
    }

    PhysicsServer::PhysicsServer()
    {
        Initialize();
    }

    PhysicsServer::~PhysicsServer()
    {
        Clean();
    }

    void PhysicsServer::Initialize()
    {
        pxFoundation = PxCreateFoundation(PX_PHYSICS_VERSION,
            pxAllocatorCallback,
            pxErrorCallback);
        pxFoundation->setErrorLevel(physx::PxErrorCode::eDEBUG_INFO);
        if (pxFoundation == nullptr)
            throw std::runtime_error("PxFoundation creation failed!");

        pxPvd = physx::PxCreatePvd(*pxFoundation);
        if (pxPvd == nullptr)
            throw std::runtime_error("PxPvd creation failed!");
        physx::PxPvdTransport* transport = physx::PxDefaultPvdSocketTransportCreate("127.0.0.1", 5425, 10);
        if (transport == nullptr)
            throw std::runtime_error("PxPvd transport creation failed!");
        pxPvd->connect(*transport, physx::PxPvdInstrumentationFlag::eALL);

        pxPhysics = PxCreatePhysics(PX_PHYSICS_VERSION, *pxFoundation, physx::PxTolerancesScale(), true, pxPvd);
        if (pxPhysics == nullptr)
            throw std::runtime_error("PxPhysics creation failed!");

        physx::PxSceneDesc sceneDesc(pxPhysics->getTolerancesScale());
        sceneDesc.gravity = physx::PxVec3(0.0f, -9.81f, 0.0f);

        pxDispatcher = physx::PxDefaultCpuDispatcherCreate(2);
        if (pxDispatcher == nullptr)
            throw std::runtime_error("PxDefaultCPUDispatcher creation failed!");

        sceneDesc.cpuDispatcher = pxDispatcher;
        sceneDesc.filterShader = physx::PxDefaultSimulationFilterShader;
        pxScene = pxPhysics->createScene(sceneDesc);
        if (pxScene == nullptr)
            throw std::runtime_error("PxScene creation failed!");
        

        pxMaterial = pxPhysics->createMaterial(0.5f, 0.5f, 0.6f);
        if (pxMaterial == nullptr)
            throw std::runtime_error("Default PxMaterial creation failed!");

#ifdef DEBUG
        physx::PxPvdSceneClient* pvdClient = pxScene->getScenePvdClient();
        if (pvdClient)
        {
            pvdClient->setScenePvdFlag(physx::PxPvdSceneFlag::eTRANSMIT_CONSTRAINTS, true);
            pvdClient->setScenePvdFlag(physx::PxPvdSceneFlag::eTRANSMIT_CONTACTS, true);
            pvdClient->setScenePvdFlag(physx::PxPvdSceneFlag::eTRANSMIT_SCENEQUERIES, true);
        }
#endif
        pxControllerManager = PxCreateControllerManager(*pxScene);
        if (pxControllerManager == nullptr)
            throw std::runtime_error("PxControllerManager creation failed!");
    }

    void PhysicsServer::Clean()
    {
        PX_RELEASE(pxScene);
        PX_RELEASE(pxDispatcher);
        PX_RELEASE(pxPhysics);
        if (pxPvd) {
            physx::PxPvdTransport* transport = pxPvd->getTransport();
            PX_RELEASE(pxPvd);
            PX_RELEASE(transport);
        }
        //pxControllerManager->purgeControllers();
        //PX_RELEASE(pxControllerManager);
        PX_RELEASE(pxFoundation);
    }

    void PhysicsServer::StepPhysics(double delta)
    {
        pxScene->simulate((float)delta);
        pxScene->fetchResults(true);
    }
}
