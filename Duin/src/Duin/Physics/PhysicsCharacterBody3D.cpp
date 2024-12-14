#include "dnpch.h"
#include "PhysicsCharacterBody3D.h"

#include "Duin/Core/Application.h"
#include "Duin/Physics/Physics3DServer.h"

constexpr int MAX_HIT_REPORTS = 8;

namespace duin {
    ControllerHitReport::ControllerHitReport(CharacterCollisionBody3D& owner)
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
    CharacterCollisionBody3D::CharacterCollisionBody3D(Physics3DServer& server)
        : server(server), controllerHitReportCallback(*this)
    {
        pxMaterial = server.CreateMaterial(0.5f, 0.5f, 0.5f);

        physx::PxCapsuleControllerDesc pxDesc = desc.ToPhysX();
        pxDesc.material = pxMaterial;
        pxDesc.reportCallback = &controllerHitReportCallback;

        CreatePxController(pxDesc);
        pxController->setFootPosition(pxDesc.position);
    }

    CharacterCollisionBody3D::CharacterCollisionBody3D(Physics3DServer& server, CharacterBody3DDesc desc, PhysicsMaterial material)
        : server(server), desc(desc), controllerHitReportCallback(*this)
    {
        pxMaterial = server.CreateMaterial(material);

        physx::PxCapsuleControllerDesc pxDesc = desc.ToPhysX();
        pxDesc.material = pxMaterial;
        pxDesc.reportCallback = &controllerHitReportCallback;

        CreatePxController(pxDesc);
        pxController->setFootPosition(pxDesc.position);
    }

    CharacterCollisionBody3D::~CharacterCollisionBody3D()
    {
        //if (pxController)
    }

    const CharacterBody3DDesc CharacterCollisionBody3D::GetDescriptor() const
    {
        return desc;
    }

    physx::PxController* CharacterCollisionBody3D::GetPxController() const
    {
        return pxController;
    }

    void CharacterCollisionBody3D::SetShapeHitCallback(std::function<void(const physx::PxControllerShapeHit&)> OnShapeHitCallback)
    {
        controllerHitReportCallback.OnShapeHitCallback = OnShapeHitCallback;
    }

    void CharacterCollisionBody3D::SetControllerHitCallback(std::function<void(const physx::PxControllersHit&)> OnControllerHitCallback)
    {
        controllerHitReportCallback.OnControllerHitCallback = OnControllerHitCallback;
    }

    void CharacterCollisionBody3D::SetObstacleHitCallback(std::function<void(const physx::PxControllerObstacleHit&)> OnObstacleHitCallback)
    {
        controllerHitReportCallback.OnObstacleHitCallback = OnObstacleHitCallback;
    }

    Vector3 CharacterCollisionBody3D::GetPosition()
    {
        return Vector3(pxController->getPosition());
    }

    Vector3 CharacterCollisionBody3D::GetFootPosition()
    {
        return Vector3(pxController->getFootPosition());
    }

    Vector3 CharacterCollisionBody3D::GetCurrentVelocity()
    {
        return currentVelocity;
    }

    void CharacterCollisionBody3D::Move(Vector3 displacement, double delta)
    {
        // TODO refactor and clean!

        double timeSinceLastMove = ::GetTime() - timeWhenLastMoved;
        timeSinceLastMove = delta;

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

        OnFloorShapeCast(timeSinceLastMove);

        timeWhenLastMoved = ::GetTime();
    }

    int CharacterCollisionBody3D::IsOnFloor()
    {
        return isOnFloor;
    }

    int CharacterCollisionBody3D::IsOnFloorOnly()
    {
        return 0;
    }

    int CharacterCollisionBody3D::OnFloorShapeCast(double delta)
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

    void CharacterCollisionBody3D::CreatePxController(physx::PxCapsuleControllerDesc pxDesc)
    {
        pxController = server.pxControllerManager->createController(pxDesc);
    }

}
