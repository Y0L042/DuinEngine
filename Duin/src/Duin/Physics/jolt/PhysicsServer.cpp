#include "dnpch.h"
#include "PhysicsServer.h"

#include "Duin/Core/Debug/DNLog.h"
#include "Duin/Core/Debug/DNAssert.h"

#include <Jolt/Physics/Body/BodyCreationSettings.h>
#include <Jolt/Physics/Collision/Shape/PlaneShape.h>

JPH_SUPPRESS_WARNINGS

static void TraceImpl(const char *inFMT, ...)
{
    va_list list;
    va_start(list, inFMT);
    char buffer[1024];
    vsnprintf(buffer, sizeof(buffer), inFMT, list);
    va_end(list);

    DN_CORE_FATAL("{}", buffer);
}

static bool AssertFailedImpl(const char *inExpression, const char *inMessage, const char *inFile, JPH::uint inLine)
{
    DN_CORE_FATAL("{}({}) ({}) {}", inFile, inLine, inExpression, inMessage != nullptr ? inMessage : "");
    return true;
};

duin::PhysicsServer &duin::PhysicsServer::Get()
{
    static duin::PhysicsServer server;
    return server;
}

duin::PhysicsServer::PhysicsServer()
{
    Initialize();
}

duin::PhysicsServer::~PhysicsServer()
{
}

JPH::BodyInterface &duin::PhysicsServer::BodyInterface()
{
    return physicsSystem.GetBodyInterface();
}

void duin::PhysicsServer::Initialize()
{
    JPH::RegisterDefaultAllocator();

    JPH::Trace = TraceImpl;
    JPH_IF_ENABLE_ASSERTS(JPH::AssertFailed = AssertFailedImpl;)

    JPH::Factory::sInstance = new JPH::Factory();
    JPH::RegisterTypes();

    tempAllocator = std::make_unique<JPH::TempAllocatorImpl>(10 * 1024 * 1024);
    jobSystem = std::make_unique<JPH::JobSystemThreadPool>(
        JPH::cMaxPhysicsJobs, JPH::cMaxPhysicsBarriers, std::thread::hardware_concurrency() - 1);

    physicsSystem.Init(
        /* inMaxBodies */ cMaxBodies,
        /* inNumBodyMutexes */ cNumBodyMutexes,
        /* inMaxBodyPairs */ cMaxBodyPairs,
        /* inMaxContactConstraints */ cMaxContactConstraints,
        /* inBroadPhaseLayerInterface */ broadPhaseLayerInterface,
        /* inObjectVsBroadPhaseLayerFilter */ objectVsBroadphaseLayerFilter,
        /* inObjectLayerPairFilter */ objectVsObjectLayerFilter);

    bodyInterface = &physicsSystem.GetBodyInterface();

    debugRenderer.Initialize();
    JPH::DebugRenderer::sInstance = &debugRenderer.core;

    // TODO %optional%
    physicsSystem.SetBodyActivationListener(&bodyActivationListener);
    physicsSystem.SetContactListener(&contactListener);
}

void duin::PhysicsServer::Clean()
{
}

void duin::PhysicsServer::StepPhysics(double delta)
{
    static size_t x = 0;
    physicsSystem.Update(cDeltaTime, cCollisionSteps, tempAllocator.get(), jobSystem.get());
}

void duin::PhysicsServer::CreatePlane(const Vector3 &normal, const float height)
{
    BodyInterface().CreateAndAddBody(
        JPH::BodyCreationSettings(
            new JPH::PlaneShape(JPH::Plane(JPH::Vec3(normal.x, normal.y, normal.z).Normalized(), height), nullptr),
            JPH::RVec3(0, 0, 0),
            JPH::Quat::sIdentity(),
            JPH::EMotionType::Static,
            Layers::NON_MOVING),
        JPH::EActivation::DontActivate);
}

void duin::PhysicsServer::CreateBox(const Vector3 &position, const Vector3 &size)
{
    BodyInterface().CreateAndAddBody(
        JPH::BodyCreationSettings(
            new JPH::BoxShape(JPH::Vec3(size.x / 2.0f, size.y / 2.0f, size.z / 2.0f)),
            JPH::RVec3(position.x, position.y, position.z),
            JPH::Quat::sRotation(JPH::Vec3::sAxisZ(), 0.25f * JPH::JPH_PI),
            JPH::EMotionType::Dynamic,
            Layers::MOVING),
        JPH::EActivation::Activate);
}

void duin::PhysicsServer::DebugDrawBodies()
{
    JPH::BodyManager::DrawSettings settings;
    JPH::DebugRenderer *r = static_cast<JPH::DebugRenderer *>(&debugRenderer.core);
    physicsSystem.DrawBodies(settings, r);
}
