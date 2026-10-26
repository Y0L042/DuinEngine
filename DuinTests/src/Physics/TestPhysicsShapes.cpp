#include <doctest.h>
#include <Duin/Physics/jolt/PhysicsServer.h>
#include <Duin/Physics/jolt/CollisionShape.h>
#include <Duin/Core/Maths/DuinMaths.h>
#include <array>
#include <cmath>
#include <vector>

namespace TestPhysicsShapes
{
struct PhysicsPeek : duin::PhysicsServer
{
    static JPH::uint BodyCount()
    {
        duin::PhysicsServer &server = duin::PhysicsServer::Get();
        JPH::PhysicsSystem &system = server.*(&PhysicsPeek::physicsSystem);
        return system.GetNumBodies();
    }
};

static std::vector<duin::Vector3> CubePoints()
{
    std::vector<duin::Vector3> pts;
    for (int x = -1; x <= 1; x += 2)
        for (int y = -1; y <= 1; y += 2)
            for (int z = -1; z <= 1; z += 2)
                pts.push_back(duin::Vector3((float)x, (float)y, (float)z));
    return pts;
}

static duin::PxMesh QuadMesh()
{
    duin::PxMesh mesh;
    mesh.vertices = {
        duin::Vector3(-1.0f, 0.0f, -1.0f),
        duin::Vector3(1.0f, 0.0f, -1.0f),
        duin::Vector3(1.0f, 0.0f, 1.0f),
        duin::Vector3(-1.0f, 0.0f, 1.0f),
    };
    mesh.triangles = {std::array<uint32_t, 3>{0, 1, 2}, std::array<uint32_t, 3>{0, 2, 3}};
    return mesh;
}

TEST_SUITE("Physics - Collision shapes")
{
    TEST_CASE("Primitive shapes report their type")
    {
        duin::PhysicsServer::Get();
        CHECK(duin::CollisionShape(duin::PxBox{}).GetType() == duin::CollisionShapeType::Box);
        CHECK(duin::CollisionShape(duin::PxSphere{}).GetType() == duin::CollisionShapeType::Sphere);
        CHECK(duin::CollisionShape(duin::PxCapsule{}).GetType() == duin::CollisionShapeType::Capsule);
        CHECK(duin::CollisionShape(duin::PxCylinder{}).GetType() == duin::CollisionShapeType::Cylinder);
        CHECK(duin::CollisionShape(duin::PxTriangle{}).GetType() == duin::CollisionShapeType::Triangle);
        CHECK(duin::CollisionShape(duin::PxSquare{}).GetType() == duin::CollisionShapeType::Square);
    }

    TEST_CASE("Convex hull and mesh descriptors map to the hull and mesh shape types")
    {
        duin::PhysicsServer::Get();
        duin::PxConvexHull hull;
        hull.points = CubePoints();
        CHECK(duin::CollisionShape(hull).GetType() == duin::CollisionShapeType::ConvexMesh);
        CHECK(duin::CollisionShape(QuadMesh()).GetType() == duin::CollisionShapeType::TriangleMesh);
    }

    TEST_CASE("Static body from a cube convex hull is added to the world")
    {
        duin::PhysicsServer &server = duin::PhysicsServer::Get();
        duin::PxConvexHull hull;
        hull.points = CubePoints();
        duin::CollisionShape shape(hull);

        JPH::uint before = PhysicsPeek::BodyCount();
        server.CreateStaticBody(shape, duin::Vector3(0.0f, -100.0f, 0.0f));
        CHECK(PhysicsPeek::BodyCount() == before + 1);
    }

    TEST_CASE("Degenerate convex hull produces no body")
    {
        duin::PhysicsServer &server = duin::PhysicsServer::Get();

        SUBCASE("Empty point cloud")
        {
            duin::PxConvexHull hull;
            duin::CollisionShape shape(hull);
            JPH::uint before = PhysicsPeek::BodyCount();
            server.CreateStaticBody(shape, duin::Vector3(0.0f, -100.0f, 0.0f));
            CHECK(PhysicsPeek::BodyCount() == before);
        }
        SUBCASE("Too few points")
        {
            duin::PxConvexHull hull;
            hull.points = {duin::Vector3(0.0f, 0.0f, 0.0f), duin::Vector3(1.0f, 0.0f, 0.0f)};
            duin::CollisionShape shape(hull);
            JPH::uint before = PhysicsPeek::BodyCount();
            server.CreateStaticBody(shape, duin::Vector3(0.0f, -100.0f, 0.0f));
            CHECK(PhysicsPeek::BodyCount() == before);
        }
    }

    TEST_CASE("Static body from a triangle mesh is added to the world")
    {
        duin::PhysicsServer &server = duin::PhysicsServer::Get();
        duin::CollisionShape shape(QuadMesh());

        JPH::uint before = PhysicsPeek::BodyCount();
        server.CreateStaticBody(shape, duin::Vector3(0.0f, -110.0f, 0.0f));
        CHECK(PhysicsPeek::BodyCount() == before + 1);
    }

    TEST_CASE("Empty triangle mesh produces no body")
    {
        duin::PhysicsServer &server = duin::PhysicsServer::Get();
        duin::CollisionShape shape(duin::PxMesh{});

        JPH::uint before = PhysicsPeek::BodyCount();
        server.CreateStaticBody(shape, duin::Vector3(0.0f, -120.0f, 0.0f));
        CHECK(PhysicsPeek::BodyCount() == before);
    }

    TEST_CASE("Static body accepts non-unit scale and rotation")
    {
        duin::PhysicsServer &server = duin::PhysicsServer::Get();
        duin::CollisionShape box(duin::PxBox{});

        JPH::uint before = PhysicsPeek::BodyCount();
        server.CreateStaticBody(box, duin::Vector3(0.0f, -130.0f, 0.0f), duin::Vector3(2.0f, 1.0f, 3.0f));
        server.CreateStaticBody(
            box,
            duin::Vector3(5.0f, -130.0f, 0.0f),
            duin::Vector3(1.0f, 1.0f, 1.0f),
            duin::QuaternionFromAxisAngle(duin::Vector3(0.0f, 1.0f, 0.0f), 0.5f));
        CHECK(PhysicsPeek::BodyCount() == before + 2);
    }

    TEST_CASE("The same shape can back several static bodies")
    {
        duin::PhysicsServer &server = duin::PhysicsServer::Get();
        duin::PxConvexHull hull;
        hull.points = CubePoints();
        duin::CollisionShape shape(hull);

        JPH::uint before = PhysicsPeek::BodyCount();
        server.CreateStaticBody(shape, duin::Vector3(10.0f, -140.0f, 0.0f));
        server.CreateStaticBody(shape, duin::Vector3(20.0f, -140.0f, 0.0f), duin::Vector3(2.0f, 2.0f, 2.0f));
        CHECK(PhysicsPeek::BodyCount() == before + 2);
    }
}
} // namespace TestPhysicsShapes
