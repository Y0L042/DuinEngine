#include "dnpch.h"
#include "Physics3DServer.h"

#include "Duin/Core/Debug/DNLog.h"

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

    void Physics3DServer::StepPhysics(double delta)
    {
        pxScene->simulate((float)delta);
        pxScene->fetchResults(true);
    }

    CharacterBody3D::CharacterBody3D(Physics3DServer& manager, CharacterBody3DDesc desc)
        : manager(manager)
    {
        controller = manager.pxControllerManager->createController(desc.ToPhysX());
    }

    CharacterBody3D::~CharacterBody3D()
    {}

    void CharacterBody3D::Move(Vector3 displacement, double delta)
    {
        physx::PxControllerCollisionFlags collisionFlags = controller->move(
                                    displacement.ToPhysX(),
                                    0.000001f,
                                    (float)delta,
                                    filters,
                                    obstacles
                                  );
    }

    StaticCollisionPlane::StaticCollisionPlane(Physics3DServer& manager)
        : manager(manager)
    {
        physx::PxMaterial *pxMaterial = manager.pxPhysics->createMaterial(0.5f, 0.5f, 0.6f);
        physx::PxRigidStatic *groundPlane = physx::PxCreatePlane(*manager.pxPhysics, physx::PxPlane(0, 1, 0, 0), *pxMaterial);
        manager.pxScene->addActor(*groundPlane);
    }
}
