#include "dnpch.h"
#include "CollisionShape.h"

duin::CollisionShape::CollisionShape(duin::CollisionGeometryVariant geometryVariant, duin::PhysicsMaterial physicsMaterial)
    : geometryVariant(geometryVariant), physicsMaterial(physicsMaterial)
{
}

duin::CollisionShape::~CollisionShape()
{
}

duin::CollisionGeometryVariant &duin::CollisionShape::GetGeometryVariant()
{
    return geometryVariant;
}
