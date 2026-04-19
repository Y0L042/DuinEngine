#pragma once

#include <variant>

#include "PhysicsMaterial.h"
#include "CollisionGeometry.h"
#include <rfl.hpp>

namespace duin
{

using CollisionGeometryVariant =
    std::variant<BoxGeometry, SphereGeometry, CapsuleGeometry, PlaneGeometry, ConvexMeshGeometry, TriangleMeshGeometry>;

class CollisionShape
{
  public:
    CollisionShape(CollisionGeometryVariant geometryVariant, PhysicsMaterial physicsMaterial = PhysicsMaterial());
    ~CollisionShape();

    CollisionGeometryVariant &GetGeometryVariant();

    using ReflectionType = struct
    {
        CollisionGeometryVariant cgVar;
        PhysicsMaterial pmat;
    };
    CollisionShape(const ReflectionType &impl) : CollisionShape(impl.cgVar, impl.pmat)
    {
    }
    const ReflectionType &reflection() const
    {
        return ReflectionType{geometryVariant, physicsMaterial};
    }

  private:
    friend class StaticBody;
    friend class KinematicBody;
    friend class DynamicBody;

    CollisionGeometryVariant geometryVariant;
    PhysicsMaterial physicsMaterial;
};

} // namespace duin
