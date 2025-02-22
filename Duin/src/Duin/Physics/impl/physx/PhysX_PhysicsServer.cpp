#include "dnpch.h"

#include <iostream>

#include "PhysX_PhysicsServer.h"

namespace duin {
    PhysXPhysicsServer::PhysXPhysicsServer()
    {
        try {
            Initialize();
        } catch (const std::runtime_error& e) {
            DN_CORE_ERROR("Caught {0}", e.what());
        };
    }

    PhysXPhysicsServer::~PhysXPhysicsServer()
    {
        Clean();
    }

    void PhysXPhysicsServer::Initialize()
    {
        pxFoundation = PxCreateFoundation(PX_PHYSICS_VERSION,
            pxAllocatorCallback,
            pxErrorCallback);

        if (pxFoundation == nullptr)
            throw std::runtime_error("PxFoundation creation failed!");

#ifdef DEBUG
        pxPvd = physx::PxCreatePvd(*pxFoundation);
        if (pxPvd == nullptr)
            throw std::runtime_error("PxPvd creation failed!");
        physx::PxPvdTransport* transport = physx::PxDefaultPvdSocketTransportCreate("127.0.0.1", 5425, 10);
        if (transport == nullptr)
            throw std::runtime_error("PxPvd transport creation failed!");
        pxPvd->connect(*transport, physx::PxPvdInstrumentationFlag::eALL);
#endif

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

    void PhysXPhysicsServer::Clean()
    {
        PX_RELEASE(pxScene);
        PX_RELEASE(pxDispatcher);
        PX_RELEASE(pxPhysics);
        if (pxPvd) {
            physx::PxPvdTransport* transport = pxPvd->getTransport();
            PX_RELEASE(pxPvd);
            PX_RELEASE(transport);
        }
        PX_RELEASE(pxFoundation);
    }

    void PhysXPhysicsServer::StepPhysics(double delta)
    {
        pxScene->simulate((float)delta);
        pxScene->fetchResults(true);
    }
}
