#pragma once

#include <cstdint>
#include <iostream>
#include <cstdarg>
#include <thread>

#include <Jolt/Jolt.h>
#include <Jolt/RegisterTypes.h>
#include <Jolt/Core/Factory.h>
#include <Jolt/Core/TempAllocator.h>
#include <Jolt/Core/JobSystemThreadPool.h>
#include <Jolt/Physics/PhysicsSettings.h>
#include <Jolt/Physics/PhysicsSystem.h>
#include <Jolt/Physics/Collision/Shape/BoxShape.h>
#include <Jolt/Physics/Collision/Shape/SphereShape.h>
#include <Jolt/Physics/Body/BodyCreationSettings.h>
#include <Jolt/Physics/Body/BodyActivationListener.h>
#include <Jolt/Core/IssueReporting.h>

#include "JoltCollisionSolverInterfaces.h"
#include "PhysicsDebugRenderer.h"

namespace duin
{

enum PhysicsServerError
{
    SUCCESS = 0,
    INIT_FAILED,
    MATERIAL_CREATION_FAILED
};

class UUID;
class CollisionObject;
class CollisionShape;
class CharacterBody;
class PhysicsBody;

class PhysicsServer
{
  public:
    static PhysicsServer &Get();

    void Initialize();
    void Clean();
    void StepPhysics(double delta);

    void CreatePlane(const Vector3& normal, const float height);
    void CreateBox(const Vector3& position, const Vector3& size);

    void DebugDrawBodies();

  protected:
    friend class StaticBody;
    friend class KinematicBody;
    friend class DynamicBody;
    friend class CharacterBody;
    friend class CollisionShape;
    friend class PhysicsMaterial;

    const JPH::uint cMaxBodies = 1024;
    const JPH::uint cNumBodyMutexes = 0;
    const JPH::uint cMaxBodyPairs = 1024;
    const JPH::uint cMaxContactConstraints = 1024;
    const float cDeltaTime = 1.0f / 60.0f;

    int cCollisionSteps = 1;

    PhysicsDebugRenderer debugRenderer;

    std::unique_ptr<JPH::TempAllocatorImpl> tempAllocator;
    std::unique_ptr<JPH::JobSystemThreadPool> jobSystem;
    BPLayerInterfaceImpl broadPhaseLayerInterface;
    ObjectVsBroadPhaseLayerFilterImpl objectVsBroadphaseLayerFilter;
    ObjectLayerPairFilterImpl objectVsObjectLayerFilter;
    JPH::PhysicsSystem physicsSystem;
    JPH::BodyInterface *bodyInterface;

    // TODO %optional%
    MyBodyActivationListener bodyActivationListener;
    MyContactListener contactListener;

    PhysicsServer();
    ~PhysicsServer();

    JPH::BodyInterface &BodyInterface();
};

} // namespace duin
