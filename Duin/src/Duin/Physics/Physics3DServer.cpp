#include "dnpch.h"
#include "Physics3DServer.h"

#include "Duin/Core/Debug/DNLog.h"
#include "Duin/Core/Application.h"

constexpr int MAX_HIT_REPORTS = 8;

namespace duin {
    ControllerHitReport::ControllerHitReport(CharacterBody3D& owner)
        : owner(owner)
    {
        QueuePostFrameCallback(std::function<void(void)>([this]() { CleanHitReports(); }));

        currentFrameShapeHitReports = &shapeHitReports_1;
        previousFrameShapeHitReports = &shapeHitReports_2;

        currentFrameControllerHitReports = &controllerHitReports_1;
        previousFrameControllerHitReports = &controllerHitReports_2;

        currentFrameObstacleHitReports = &obstacleHitReports_1;
        previousFrameObstacleHitReports = &obstacleHitReports_2;
    }

    void ControllerHitReport::onShapeHit(const physx::PxControllerShapeHit& hit)
    {
        OnShapeHit(hit);
        if (OnShapeHitCallback) 
        {
            OnShapeHitCallback(hit);
        }
        if (currentFrameShapeHitReports->size() < MAX_HIT_REPORTS)
        {
            currentFrameShapeHitReports->push_back(hit);
        }
    }

    void ControllerHitReport::onControllerHit(const physx::PxControllersHit& hit)
    {
        OnControllerHit(hit);
        if (OnControllerHitCallback)
        {
            OnControllerHitCallback(hit);
        }
        if (currentFrameControllerHitReports->size() < MAX_HIT_REPORTS)
        {
            currentFrameControllerHitReports->push_back(hit);
        }
    }

    void ControllerHitReport::onObstacleHit(const physx::PxControllerObstacleHit& hit)
    {
        OnObstacleHit(hit);
        if (OnObstacleHitCallback)
        {
            OnObstacleHitCallback(hit);
        }
        if (currentFrameObstacleHitReports->size() < MAX_HIT_REPORTS)
        {
            currentFrameObstacleHitReports->push_back(hit);
        }
    }

    const std::vector<physx::PxControllerShapeHit>& ControllerHitReport::GetShapeHitReports()
    {
        return *previousFrameShapeHitReports;
    }

    const std::vector<physx::PxControllersHit>& ControllerHitReport::GetControllerHitReports()
    {
        return *previousFrameControllerHitReports;
    }

    const std::vector<physx::PxControllerObstacleHit>& ControllerHitReport::GetObstacleHitReports()
    {
        return *previousFrameObstacleHitReports;
    }

    void ControllerHitReport::CleanHitReports()
    {
        previousFrameShapeHitReports->clear();
        previousFrameControllerHitReports->clear();
        previousFrameObstacleHitReports->clear();

        auto* swapShapePtrs = currentFrameShapeHitReports;
        currentFrameShapeHitReports = previousFrameShapeHitReports;
        previousFrameShapeHitReports = swapShapePtrs;

        auto* swapControllerPtrs = currentFrameControllerHitReports;
        currentFrameControllerHitReports = previousFrameControllerHitReports;
        previousFrameControllerHitReports = swapControllerPtrs;

        auto* swapObstaclePtrs = currentFrameObstacleHitReports;
        currentFrameObstacleHitReports = previousFrameObstacleHitReports;
        previousFrameObstacleHitReports = swapObstaclePtrs;
    }
}





namespace duin {
    CharacterBody3D::CharacterBody3D(Physics3DServer& server)
        : server(server), controllerHitReportCallback(*this)
    {
        pxMaterial = server.CreateMaterial(0.5f, 0.5f, 0.5f);

        physx::PxCapsuleControllerDesc pxDesc = desc.ToPhysX();
        pxDesc.material = pxMaterial;
        pxDesc.reportCallback = &controllerHitReportCallback;

        CreatePxController(pxDesc);
    }

    CharacterBody3D::CharacterBody3D(Physics3DServer& server, CharacterBody3DDesc desc, PhysicsMaterial material)
        : server(server), desc(desc), controllerHitReportCallback(*this)
    {
        pxMaterial = server.CreateMaterial(material);

        physx::PxCapsuleControllerDesc pxDesc = desc.ToPhysX();
        pxDesc.material = pxMaterial;
        pxDesc.reportCallback = &controllerHitReportCallback;

        CreatePxController(pxDesc);
    }

    CharacterBody3D::~CharacterBody3D()
    {
        //if (pxController)
    }

    const CharacterBody3DDesc CharacterBody3D::GetDescriptor() const
    {
        return desc;
    }

    physx::PxController* CharacterBody3D::GetPxController() const
    {
        return pxController;
    }

    void CharacterBody3D::SetShapeHitCallback(std::function<void(const physx::PxControllerShapeHit&)> OnShapeHitCallback)
    {
        controllerHitReportCallback.OnShapeHitCallback = OnShapeHitCallback;
    }

    void CharacterBody3D::SetControllerHitCallback(std::function<void(const physx::PxControllersHit&)> OnControllerHitCallback)
    {
        controllerHitReportCallback.OnControllerHitCallback = OnControllerHitCallback;
    }

    void CharacterBody3D::SetObstacleHitCallback(std::function<void(const physx::PxControllerObstacleHit&)> OnObstacleHitCallback)
    {
        controllerHitReportCallback.OnObstacleHitCallback = OnObstacleHitCallback;
    }

    Vector3 CharacterBody3D::GetPosition()
    {
        return Vector3(pxController->getPosition());
    }

    Vector3 CharacterBody3D::GetFootPosition()
    {
        return Vector3(pxController->getFootPosition());
    }

    Vector3 CharacterBody3D::GetCurrentVelocity()
    {
        return currentVelocity;
    }

    void CharacterBody3D::Move(Vector3 displacement)
    {
        timeSinceLastMove = ::GetTime() - timeSinceLastMove;

        Vector3 oldPos = Vector3(pxController->getPosition());
        physx::PxControllerCollisionFlags collisionFlags = pxController->move(
            displacement.ToPhysX(),
            0.00001f,
            (float)timeSinceLastMove,
            pxFilters,
            pxObstacles
        );

        Vector3 newPos = Vector3(pxController->getPosition());
        Vector3 distanceMoved = Vector3Subtract(newPos, oldPos);
        
        currentVelocity = Vector3Scale(distanceMoved, 1.0f / timeSinceLastMove);

        timeSinceLastMove = ::GetTime();

        OnFloorShapeCast(timeSinceLastMove);
    }

    int CharacterBody3D::IsOnFloor()
    {
        return isOnFloor;
    }

    int CharacterBody3D::IsOnFloorOnly()
    {
        return 0;
    }

    int CharacterBody3D::OnFloorShapeCast(double delta)
    {
        float maxSlopeAngleDegrees = 50.0f;

        float sphereRadius = desc.radius * 0.95f;

        float footOffset = 0.25f;
        float castDistance = 0.00001f;

        // Position the sphere at the character's foot position with an upward offset
        Vector3 castOrigin = Vector3(pxController->getFootPosition() + physx::PxExtendedVec3(0.0, (double)(footOffset + sphereRadius), 0.0));

        // Direction to sweep the shape (downwards)
        Vector3 castDirection = Vector3(physx::PxVec3(0.0f, -1.0f, 0.0f));

        // Define the shape to cast (sphere in this case)
        
        physx::PxSphereGeometry sphereGeometry(sphereRadius);

        // Define the pose of the sphere (position and orientation)
        physx::PxTransform castPose(castOrigin.ToPhysX());

        // Prepare the hit buffer
        physx::PxSweepBuffer hit;

        // Perform the shape cast (sweep)
        bool hasHit = pxController->getScene()->sweep(
            sphereGeometry,
            castPose,
            castDirection.ToPhysX(),
            (castDistance + sphereRadius),
            hit,
            physx::PxHitFlag::eDEFAULT,
            physx::PxQueryFilterData(physx::PxQueryFlag::eSTATIC)
        );

        // If the shape cast hits an object
        int onFloor = 0;
        if (hasHit)
        {
            float maxSlopeCosine = physx::PxCos(physx::PxPi * maxSlopeAngleDegrees / 180.0f);
            if (hit.block.normal.dot(physx::PxVec3(0, 1, 0)) >= maxSlopeCosine)
            {
                onFloor = 1;
            }
        }

        
        if (!onFloor)
        {
            timeSinceOnFloor += delta;
        }
        else
        {
            timeSinceOnFloor = 0;
        }

        if (isOnFloor) {
            if (!onFloor) {
                if (timeSinceOnFloor > onFloorGrace) {
                    isOnFloor = onFloor;
                }
            }
        }
        else {
            isOnFloor = onFloor;
        }
        

        //isOnFloor = onFloor;

        return onFloor;
    }

    void CharacterBody3D::CreatePxController(physx::PxCapsuleControllerDesc pxDesc)
    {
        pxController = server.pxControllerManager->createController(pxDesc);
    }

}





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
