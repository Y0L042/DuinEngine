#include "dnpch.h"
#include "CollisionShape.h"
#include <Jolt/Physics/Collision/Shape/BoxShape.h>
#include <Jolt/Physics/Collision/Shape/SphereShape.h>
#include <Jolt/Physics/Collision/Shape/PlaneShape.h>
#include <Jolt/Physics/Collision/Shape/CapsuleShape.h>
#include <Jolt/Physics/Collision/Shape/CylinderShape.h>
#include <Jolt/Physics/Collision/Shape/TriangleShape.h>
#include <Jolt/Physics/Collision/Shape/ConvexHullShape.h>
#include <Jolt/Physics/Collision/Shape/MeshShape.h>

duin::CollisionShape::CollisionShape(CollisionShapeDesc desc, PhysicsMaterial material)
    : shapeDesc(desc)
{
    GetJoltShape<JPH::Shape>();
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
        return CollisionShapeType::Cylinder;
    case 5:
        return CollisionShapeType::Triangle;
    case 6:
        return CollisionShapeType::ConvexMesh;
    case 7:
        return CollisionShapeType::TriangleMesh;
    case 8:
        return CollisionShapeType::Square;
    default:
        return CollisionShapeType::Box;
    };
}