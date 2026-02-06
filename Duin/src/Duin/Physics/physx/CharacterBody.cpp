#include "dnpch.h"
#include "CharacterBody.h"

#include "PhysicsServer.h"
#include "Duin/Core/Application.h"

namespace duin
{
std::shared_ptr<CharacterBody> CharacterBody::Create(CharacterBodyDesc desc)
{
    return std::make_shared<CharacterBody>(desc);
}

CharacterBody::CharacterBody(CharacterBodyDesc desc) : desc(desc)
{
    PhysicsServer &server = PhysicsServer::Get();
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

CharacterBody::~CharacterBody()
{
    // TODO
    // if (pxController) {
    //    pxController->release();
    //    pxController = nullptr;
    //}
}

Vector3 CharacterBody::GetPosition()
{
    return Vector3(pxController->getPosition());
}

Vector3 CharacterBody::GetFootPosition()
{
    return Vector3(pxController->getFootPosition());
}

Vector3 CharacterBody::GetCurrentVelocity()
{
    return currentVelocity;
}

int CharacterBody::IsOnFloor()
{
    return isOnFloor;
}

int CharacterBody::IsOnFloorOnly()
{
    return 0;
}

void CharacterBody::Move(Vector3 displacement, double delta)
{
    double timeSinceLastMove = GetTicksMilli() - timeWhenLastMoved;
    timeSinceLastMove = delta;

    Vector3 oldPos = Vector3(pxController->getPosition());
    physx::PxControllerCollisionFlags collisionFlags =
        pxController->move(displacement.ToPhysX(), 0.00001f, (float)timeSinceLastMove, pxFilters, pxObstacles);

    Vector3 newPos = Vector3(pxController->getPosition());
    Vector3 distanceMoved = Vector3Subtract(newPos, oldPos);

    currentVelocity = Vector3Scale(distanceMoved, (float)(1.0 / timeSinceLastMove));

    OnFloorShapeCast(timeSinceLastMove);

    timeWhenLastMoved = GetTicksMilli();
}

void CharacterBody::OnShapeHit()
{
}

void CharacterBody::OnCharacterHit()
{
}

void CharacterBody::OnObstacleHit()
{
}

int CharacterBody::OnFloorShapeCast(double delta)
{
    float maxSlopeAngleDegrees = 50.0f;

    float sphereRadius = desc.radius * 0.95f;

    float footOffset = 0.25f;
    float castDistance = 0.00001f;

    // Position the sphere at the character's foot position with an upward offset
    Vector3 castOrigin =
        Vector3(pxController->getFootPosition() + physx::PxExtendedVec3(0.0, (double)(footOffset + sphereRadius), 0.0));

    // Direction to sweep the shape (downwards)
    Vector3 castDirection = Vector3(physx::PxVec3(0.0f, -1.0f, 0.0f));

    // Define the shape to cast (sphere in this case)

    physx::PxSphereGeometry sphereGeometry(sphereRadius);

    // Define the pose of the sphere (position and orientation)
    physx::PxTransform castPose(castOrigin.ToPhysX());

    // Prepare the hit buffer
    physx::PxSweepBuffer hit;

    // Perform the shape cast (sweep)
    bool hasHit = pxController->getScene()->sweep(sphereGeometry, castPose, castDirection.ToPhysX(),
                                                  (castDistance + sphereRadius), hit, physx::PxHitFlag::eDEFAULT,
                                                  physx::PxQueryFilterData(physx::PxQueryFlag::eSTATIC));

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

    if (isOnFloor)
    {
        if (!onFloor)
        {
            if (timeSinceOnFloor > onFloorGrace)
            {
                isOnFloor = onFloor;
            }
        }
    }
    else
    {
        isOnFloor = onFloor;
    }

    return onFloor;
}
const CharacterBodyDesc CharacterBody::GetDescriptor() const
{
    return desc;
}
} // namespace duin
