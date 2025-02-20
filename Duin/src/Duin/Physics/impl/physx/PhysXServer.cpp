#include "dnpch.h"

#include "PhysXServer.h"

namespace duin {
    PhysXServer::PhysXServer()
    {
    }

    PhysXServer::~PhysXServer()
    {
    }

    int PhysXServer::Initialize()
    {
        pxFoundation = PxCreateFoundation(PX_PHYSICS_VERSION,
            pxAllocatorCallback,
            pxErrorCallback);

        if (pxFoundation == nullptr) {
            return 1;
        }

#ifdef DEBUG
        pxPvd = physx::PxCreatePvd(*pxFoundation);
        physx::PxPvdTransport* transport = physx::PxDefaultPvdSocketTransportCreate("127.0.0.1", 5425, 10);
        pxPvd->connect(*transport, physx::PxPvdInstrumentationFlag::eALL);
#endif

        pxPhysics = PxCreatePhysics(PX_PHYSICS_VERSION, *pxFoundation, physx::PxTolerancesScale(), true, pxPvd);
        if (pxPhysics == nullptr) {
            return PhysicsServerErrors::INIT_FAILED;
        }

        physx::PxSceneDesc sceneDesc(pxPhysics->getTolerancesScale());
        sceneDesc.gravity = physx::PxVec3(0.0f, -9.81f, 0.0f);

        pxDispatcher = physx::PxDefaultCpuDispatcherCreate(2);
        if (pxDispatcher == nullptr) {
            return PhysicsServerErrors::INIT_FAILED;
        }

        sceneDesc.cpuDispatcher = pxDispatcher;
        sceneDesc.filterShader = physx::PxDefaultSimulationFilterShader;
        pxScene = pxPhysics->createScene(sceneDesc);
        if (PhysicsServer::CatchError(pxScene, PhysicsServerErrors::INIT_FAILED)) 
            return PhysicsServerErrors::INIT_FAILED;
        

        pxMaterial = pxPhysics->createMaterial(0.5f, 0.5f, 0.6f);
        if (PhysicsServer::CatchError(pxScene, PhysicsServerErrors::INIT_FAILED)) 
            return PhysicsServerErrors::INIT_FAILED;

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
        if (pxControllerManager == nullptr) {
            return PhysicsServerErrors::INIT_FAILED;
        }

        return PhysicsServerErrors::SUCCESS;
    }

    void PhysXServer::Clean()
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

    void PhysXServer::StepPhysics(double delta)
    {
        pxScene->simulate((float)delta);
        pxScene->fetchResults(true);
    }
}
