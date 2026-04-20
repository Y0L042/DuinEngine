#include "dnpch.h"
#include "CollisionShape.h"
#include <Jolt/Physics/Collision/Shape/BoxShape.h>
#include <Jolt/Physics/Collision/Shape/SphereShape.h>
#include <Jolt/Physics/Collision/Shape/PlaneShape.h>
#include <Jolt/Physics/Collision/Shape/CapsuleShape.h>

duin::CollisionShape::CollisionShape(CollisionShapeDesc desc, PhysicsMaterial material)
{
    GetJoltShape<void *>();
}

duin::CollisionShapeType duin::CollisionShape::GetType() const
{
    switch (shapeDesc.index())
    {
    case 0:
        return CollisionShapeType::Box;
    case 1:
        return CollisionShapeType::Sphere;
    case 2:
        return CollisionShapeType::Capsule;
    case 3:
        return CollisionShapeType::Plane;
    case 4:
        return CollisionShapeType::ConvexMesh;
    case 5:
        return CollisionShapeType::TriangleMesh;
    default:
        return CollisionShapeType::Box; // fallback
    };
}