#include "dnpch.h"

#include "PhysX_CharacterBody.h"
#include "Duin/Core/Maths/DuinMaths.h"


namespace duin {

    PhysXCharacterBody::PhysXCharacterBody()
    {

    }

    PhysXCharacterBody::PhysXCharacterBody(CharacterBodyDesc desc)
        : desc(desc) 
    {
        PhysXPhysicsServer& server = GetPxServer();
        pxMaterial = server.pxPhysics->createMaterial(0.5f, 0.5f, 0.5f);

        physx::PxCapsuleControllerDesc pxDesc;
        pxDesc.height = desc.height;
        pxDesc.radius = desc.radius;
        pxDesc.slopeLimit = desc.slopeLimit;
        pxDesc.stepOffset = desc.stepOffset;
        pxDesc.contactOffset = desc.contactOffset;
        pxDesc.position = desc.position.ToPhysXd();
        pxDesc.upDirection = desc.upDirection.ToPhysX();
        pxDesc.material = pxMaterial;

        // pxDesc.reportCallback = &controllerHitReportCallback;

        pxController = server.pxControllerManager->createController(pxDesc);
        pxController->setFootPosition(pxDesc.position);
    }

    PhysXCharacterBody::~PhysXCharacterBody()
    {
        // TODO clean physx objects
    }

    Vector3 PhysXCharacterBody::GetPosition()
    {
        return Vector3(pxController->getPosition());
    }

    Vector3 PhysXCharacterBody::GetFootPosition()
    {
        return Vector3(pxController->getFootPosition());
    }

    Vector3 PhysXCharacterBody::GetCurrentVelocity()
    {
        return currentVelocity;
    }

    int PhysXCharacterBody::IsOnFloor()
    {
        return isOnFloor;
    }

    int PhysXCharacterBody::IsOnFloorOnly()
    {
        return 0;
    }

    void PhysXCharacterBody::Move(Vector3 displacement, double delta)
    {
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

    void PhysXCharacterBody::OnShapeHit()
    {
    }

    void PhysXCharacterBody::OnCharacterHit()
    {
    }

    void PhysXCharacterBody::OnObstacleHit()
    {
    }

    int PhysXCharacterBody::OnFloorShapeCast(double delta)
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

        return onFloor;
    }
}
