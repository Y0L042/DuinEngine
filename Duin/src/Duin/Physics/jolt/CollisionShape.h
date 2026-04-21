#pragma once

#include <Jolt/Jolt.h>
#include <variant>
#include <vector>
#include <array>
#include "Duin/Core/Maths/DuinMaths.h"
#include "PhysicsMaterial.h"
#include <Jolt/Physics/Collision/Shape/BoxShape.h>
#include <Jolt/Physics/Collision/Shape/SphereShape.h>
#include <Jolt/Physics/Collision/Shape/CapsuleShape.h>
#include <Jolt/Physics/Collision/Shape/PlaneShape.h>
#include <Jolt/Physics/Collision/Shape/CylinderShape.h>
#include <Jolt/Physics/Collision/Shape/TriangleShape.h>
#include <Jolt/Physics/Collision/Shape/ConvexHullShape.h>
#include <Jolt/Physics/Collision/Shape/MeshShape.h>

namespace duin
{

enum class CollisionShapeType
{
    Box,
    Sphere,
    Capsule,
    Plane,
    Cylinder,
    Triangle,
    ConvexMesh,
    TriangleMesh,
    Square,
};

struct PxBox
{
    Vector3 sides = Vector3(1.0f, 1.0f, 1.0f);
};

struct PxSphere
{
    float radius = 0.5f;
};

struct PxCapsule
{
    float height = 2.0f;
    float radius = 0.25f;
};

struct PxPlane
{
    Vector3 normal = Vector3(0.0f, 1.0f, 0.0f);
    float height = 1.0f;
};

struct PxCylinder
{
    float halfHeight   = 1.0f;
    float radius       = 0.5f;
    float convexRadius = 0.05f;
};

struct PxTriangle
{
    Vector3 v1 = Vector3(-0.5f, 0.0f, -0.5f);
    Vector3 v2 = Vector3( 0.5f, 0.0f, -0.5f);
    Vector3 v3 = Vector3( 0.0f, 0.0f,  0.5f);
    float   convexRadius = 0.0f;
};

struct PxConvexMesh
{
    std::vector<Vector3> points;
    float maxConvexRadius = 0.05f;
};

struct PxTriangleMesh
{
    std::vector<Vector3>                 vertices;
    std::vector<std::array<uint32_t, 3>> triangles;
};

// Finite flat quad — thin BoxShape (half-depth = 0.005).
struct PxSquare
{
    float width  = 1.0f;
    float height = 1.0f;
};

using CollisionShapeDesc = std::variant<
    PxBox, PxSphere, PxCapsule, PxPlane,
    PxCylinder, PxTriangle, PxConvexMesh, PxTriangleMesh, PxSquare>;

class CollisionShape
{
  public:
    CollisionShape(CollisionShapeDesc desc, PhysicsMaterial material = PhysicsMaterial());

    CollisionShapeType GetType() const;

  private:
    friend class CharacterBody;

    CollisionShapeDesc shapeDesc;
    JPH::Shape *shapePtr = nullptr;

    template <typename T>
    T *GetJoltShape()
    {
        if (shapePtr == nullptr)
        {
            switch (shapeDesc.index())
            {
            case 0: // PxBox
            {
                const PxBox &d = std::get<0>(shapeDesc);
                shapePtr = new JPH::BoxShape(JPH::Vec3(d.sides.x / 2.0f, d.sides.y / 2.0f, d.sides.z / 2.0f));
                break;
            }
            case 1: // PxSphere
            {
                const PxSphere &d = std::get<1>(shapeDesc);
                shapePtr = new JPH::SphereShape(d.radius);
                break;
            }
            case 2: // PxCapsule
            {
                const PxCapsule &d = std::get<2>(shapeDesc);
                shapePtr = new JPH::CapsuleShape(d.height / 2.0f, d.radius);
                break;
            }
            case 3: // PxPlane
            {
                const PxPlane &d = std::get<3>(shapeDesc);
                shapePtr = new JPH::PlaneShape(
                    JPH::Plane(JPH::Vec3(d.normal.x, d.normal.y, d.normal.z).Normalized(), d.height), nullptr);
                break;
            }
            case 4: // PxCylinder
            {
                const PxCylinder &d = std::get<4>(shapeDesc);
                shapePtr = new JPH::CylinderShape(d.halfHeight, d.radius, d.convexRadius);
                break;
            }
            case 5: // PxTriangle
            {
                const PxTriangle &d = std::get<5>(shapeDesc);
                shapePtr = new JPH::TriangleShape(
                    JPH::Vec3(d.v1.x, d.v1.y, d.v1.z),
                    JPH::Vec3(d.v2.x, d.v2.y, d.v2.z),
                    JPH::Vec3(d.v3.x, d.v3.y, d.v3.z),
                    d.convexRadius);
                break;
            }
            case 6: // PxConvexMesh
            {
                const PxConvexMesh &d = std::get<6>(shapeDesc);
                if (!d.points.empty())
                {
                    JPH::Array<JPH::Vec3> pts;
                    pts.reserve(d.points.size());
                    for (const auto &p : d.points)
                        pts.push_back(JPH::Vec3(p.x, p.y, p.z));
                    JPH::ConvexHullShapeSettings settings(pts.data(), (int)pts.size(), d.maxConvexRadius);
                    auto result = settings.Create();
                    if (result.IsValid())
                        shapePtr = result.Get().GetPtr();
                }
                break;
            }
            case 7: // PxTriangleMesh
            {
                const PxTriangleMesh &d = std::get<7>(shapeDesc);
                if (!d.vertices.empty() && !d.triangles.empty())
                {
                    JPH::TriangleList tris;
                    tris.reserve(d.triangles.size());
                    for (const auto &tri : d.triangles)
                        tris.push_back(JPH::Triangle(
                            JPH::Float3(d.vertices[tri[0]].x, d.vertices[tri[0]].y, d.vertices[tri[0]].z),
                            JPH::Float3(d.vertices[tri[1]].x, d.vertices[tri[1]].y, d.vertices[tri[1]].z),
                            JPH::Float3(d.vertices[tri[2]].x, d.vertices[tri[2]].y, d.vertices[tri[2]].z)));
                    JPH::MeshShapeSettings settings(tris);
                    auto result = settings.Create();
                    if (result.IsValid())
                        shapePtr = result.Get().GetPtr();
                }
                break;
            }
            case 8: // PxSquare — thin box
            {
                const PxSquare &d = std::get<8>(shapeDesc);
                shapePtr = new JPH::BoxShape(JPH::Vec3(d.width * 0.5f, 0.005f, d.height * 0.5f));
                break;
            }
            default:
                break;
            }
        }
        return static_cast<T *>(shapePtr);
    }
};

} // namespace duin
