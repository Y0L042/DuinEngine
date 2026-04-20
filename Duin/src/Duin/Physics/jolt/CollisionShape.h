#pragma once

#include <variant>
#include "Duin/Core/Maths/DuinMaths.h"
#include "PhysicsMaterial.h"
#include <Jolt/Physics/Collision/Shape/BoxShape.h>
#include <Jolt/Physics/Collision/Shape/SphereShape.h>
#include <Jolt/Physics/Collision/Shape/CapsuleShape.h>
#include <Jolt/Physics/Collision/Shape/PlaneShape.h>

namespace duin
{

enum class CollisionShapeType
{
    Box,
    Sphere,
    Capsule,
    Plane,
    ConvexMesh,
    TriangleMesh
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

struct PxConvexMesh
{
    // TODO
};

struct PxTriangleMesh
{
    // TODO
};

using CollisionShapeDesc = std::variant<PxBox, PxSphere, PxCapsule, PxPlane, PxConvexMesh, PxTriangleMesh>;

class CollisionShape
{
  public:
    CollisionShape(CollisionShapeDesc desc, PhysicsMaterial material = PhysicsMaterial());

    CollisionShapeType GetType() const;

  private:
    friend class CharacterBody;

    CollisionShapeDesc shapeDesc;
    void *shapePtr = nullptr;

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
            case 4: // PxConvexMesh
            {
                // TODO: Implement ConvexMesh shape creation
                shapePtr = nullptr;
                break;
            }
            case 5: // PxTriangleMesh
            {
                // TODO: Implement TriangleMesh shape creation
                shapePtr = nullptr;
                break;
            }
            default:
                shapePtr = nullptr;
                break;
            }
        }
        return reinterpret_cast<T *>(shapePtr);
    }
};

} // namespace duin
