#include "dnpch.h"
#include "CharacterBody.h"
#include "PhysicsServer.h"
#include "JoltConversions.h"
#include "JoltCollisionSolverInterfaces.h"
#include <Jolt/Physics/Character/CharacterVirtual.h>
#include "Duin/Core/Debug/DNLog.h"
#include "Duin/Core/Debug/DNAssert.h"

duin::CharacterBody::CharacterBody(CharacterBodyDesc bodyDesc, CollisionShapeDesc shapeDesc, Vector3 position)
    : bodyDesc(bodyDesc), shapeDesc(shapeDesc)
{
}

duin::CharacterBody::~CharacterBody()
{
}

void duin::CharacterBody::Initialize(duin::Vector3 position)
{
    CollisionShape shape(shapeDesc, PhysicsMaterial{});

    // Compute shapeHalfExtent and retrieve the Jolt shape via CollisionShape's lazy builder
    std::visit(
        [&](auto &&px) {
            using T = std::decay_t<decltype(px)>;
            if constexpr (std::is_same_v<T, PxCapsule>)
            {
                shapeHalfExtent = px.height * 0.5f + px.radius;
            }
            else if constexpr (std::is_same_v<T, PxSphere>)
            {
                shapeHalfExtent = px.radius;
            }
            else if constexpr (std::is_same_v<T, PxBox>)
            {
                shapeHalfExtent = px.sides.y * 0.5f;
            }
        },
        shapeDesc);

    JPH::Shape *joltShape = shape.GetJoltShape<JPH::Shape>();
    JPH_ASSERT(joltShape != nullptr);

    JPH::Ref<JPH::CharacterVirtualSettings> settings = new JPH::CharacterVirtualSettings();
    settings->mMass = bodyDesc.mass;
    settings->mMaxStrength = bodyDesc.maxStrength;
    settings->mMaxSlopeAngle = JPH::DegreesToRadians(bodyDesc.maxSlopeAngle);
    settings->mShape = joltShape;
    settings->mInnerBodyShape = joltShape;
    settings->mSupportingVolume = JPH::Plane(JPH::Vec3::sAxisY(), -0.05f);
    settings->mCharacterPadding = 0.02f;
    settings->mPenetrationRecoverySpeed = 1.0f;
    settings->mPredictiveContactDistance = 0.1f;
    settings->mInnerBodyLayer = Layers::MOVING;

    character = new JPH::CharacterVirtual(
        settings,
        JPH::RVec3(position.x, position.y, position.z),
        JPH::Quat::sIdentity(),
        0,
        &PhysicsServer::Get().physicsSystem);
    DN_CORE_ASSERT(character != nullptr, "Failed to create JPH::CharacterVirtual!");

    DN_CORE_INFO("CharacterBody initialized.");
}

void duin::CharacterBody::CreateBody(
    const duin::CollisionShape &shape, const duin::Vector3 &position, const duin::Quaternion &rotation)
{
}

void duin::CharacterBody::SetPosition(duin::Vector3 position)
{
    DN_CORE_ASSERT(character != nullptr, "Character is not initialized!");

    character->SetPosition(JPH::RVec3(position.x, position.y, position.z));
}

duin::Vector3 duin::CharacterBody::GetPosition()
{
    DN_CORE_ASSERT(character != nullptr, "Character is not initialized!");

    JPH::RVec3 p = character->GetPosition();
    return {(float)p.GetX(), (float)p.GetY(), (float)p.GetZ()};
}

duin::Vector3 duin::CharacterBody::GetCenterOfMassPosition()
{
    DN_CORE_ASSERT(character != nullptr, "Character is not initialized!");

    JPH::RVec3 p = character->GetCenterOfMassPosition();
    return {(float)p.GetX(), (float)p.GetY(), (float)p.GetZ()};
}

void duin::CharacterBody::SetFootPosition(duin::Vector3 position)
{
    DN_CORE_ASSERT(character != nullptr, "Character is not initialized!");

    character->SetPosition(JPH::RVec3(position.x, position.y + shapeHalfExtent, position.z));
}

duin::Vector3 duin::CharacterBody::GetFootPosition()
{
    DN_CORE_ASSERT(character != nullptr, "Character is not initialized!");

    JPH::RVec3 com = character->GetCenterOfMassPosition();
    return {(float)com.GetX(), (float)com.GetY() - shapeHalfExtent, (float)com.GetZ()};
}

duin::Vector3 duin::CharacterBody::GetCurrentVelocity()
{
    DN_CORE_ASSERT(character != nullptr, "Character is not initialized!");

    return FromJPHVec3(character->GetLinearVelocity());
}

int duin::CharacterBody::IsOnFloor()
{
    DN_CORE_ASSERT(character != nullptr, "Character is not initialized!");

    return (timeSinceOnFloor <= onFloorGrace) ? 1 : 0;
}

int duin::CharacterBody::IsOnFloorOnly()
{
    DN_CORE_ASSERT(character != nullptr, "Character is not initialized!");

    return isOnFloor;
}

void duin::CharacterBody::Move(duin::Vector3 displacement, double delta)
{
    DN_CORE_ASSERT(character != nullptr, "Character is not initialized!");

    PhysicsServer &server = PhysicsServer::Get();
    currentVelocity = displacement;
    character->SetLinearVelocity(ToJPHVec3(displacement));

    JPH::CharacterVirtual::ExtendedUpdateSettings update_settings;
    if (!bodyDesc.enableStickToFloor)
        update_settings.mStickToFloorStepDown = JPH::Vec3::sZero();
    else
        update_settings.mStickToFloorStepDown = -character->GetUp() * update_settings.mStickToFloorStepDown.Length();
    if (!bodyDesc.enableStairStepping)
        update_settings.mWalkStairsStepUp = JPH::Vec3::sZero();

    character->ExtendedUpdate(
        (float)delta,
        -character->GetUp() * server.physicsSystem.GetGravity().Length(),
        update_settings,
        server.physicsSystem.GetDefaultBroadPhaseLayerFilter(Layers::MOVING),
        server.physicsSystem.GetDefaultLayerFilter(Layers::MOVING),
        {},
        {},
        *server.tempAllocator);

    isOnFloor = character->GetGroundState() == JPH::CharacterBase::EGroundState::OnGround ? 1 : 0;
    timeSinceOnFloor = isOnFloor ? 0.0 : timeSinceOnFloor + delta;
}

void duin::CharacterBody::OnShapeHit()
{
}

void duin::CharacterBody::OnCharacterHit()
{
}

void duin::CharacterBody::OnObstacleHit()
{
}

int duin::CharacterBody::OnFloorShapeCast(double delta)
{
    return 0;
}
