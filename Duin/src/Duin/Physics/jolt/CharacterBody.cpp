#include "dnpch.h"
#include "CharacterBody.h"
#include "PhysicsServer.h"
#include "CollisionShape.h"
#include "Duin/Core/Maths/DuinMaths.h"

duin::CharacterBody::CharacterBody(CharacterBodyDesc bodyDesc, CollisionShapeDesc shapeDesc, Vector3 position)
    : bodyDesc(bodyDesc), shapeDesc(shapeDesc)
{
    CreateBody(position, Quaternion());
}

duin::CharacterBody::~CharacterBody()
{
}

void duin::CharacterBody::SetPosition(duin::Vector3 position)
{
}

duin::Vector3 duin::CharacterBody::GetPosition()
{
    return {};
}

void duin::CharacterBody::SetFootPosition(duin::Vector3 position)
{
}

duin::Vector3 duin::CharacterBody::GetFootPosition()
{
    return {};
}

duin::Vector3 duin::CharacterBody::GetCurrentVelocity()
{
    return {};
}

int duin::CharacterBody::IsOnFloor()
{
    return 0;
}

int duin::CharacterBody::IsOnFloorOnly()
{
    return 0;
}

void duin::CharacterBody::Move(duin::Vector3 displacement, double delta)
{
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

void duin::CharacterBody::Initialize(Vector3 position)
{
    CollisionShape shape(shapeDesc, PhysicsMaterial{});
    CreateBody(shape, position);

    // Create 'player' character
    JPH::Ref<JPH::CharacterVirtualSettings> settings = new JPH::CharacterVirtualSettings();
    settings->mMaxSlopeAngle = bodyDesc.maxSlopeAngle;
    settings->mMaxStrength = bodyDesc.maxStrength;
    settings->mShape = shape.GetJoltShape<JPH::Shape>();
    settings->mInnerBodyLayer = Layers::MOVING;

    character = new JPH::CharacterVirtual(settings, JPH::RVec3::sZero(), JPH::Quat::sIdentity(), 0, PhysicsServer::Get().physicsSystem);

    //character->SetCharacterVsCharacterCollision(&mCharacterVsCharacterCollision);
    //mCharacterVsCharacterCollision.Add(character);
}

void duin::CharacterBody::CreateBody(
    const CollisionShape &shape, const Vector3 &position, const Quaternion &rotation = Quaternion())

{
    // Create the collision shape
    // Pass appropriate material if needed

    // Create the Jolt body using the shape
    PhysicsServer::Get().BodyInterface().CreateAndAddBody(
        JPH::BodyCreationSettings(
            shape.GetJoltShape<JPH::Shape>(), // Use the correct Jolt shape type
            JPH::RVec3(position.x, position.y, position.z),
            JPH::Quat::sRotation(JPH::Vec3::sAxisZ(), 0.25f * JPH::JPH_PI), // Use 'rotation' if you want
            JPH::EMotionType::Dynamic,
            Layers::MOVING),
        JPH::EActivation::Activate);
}
