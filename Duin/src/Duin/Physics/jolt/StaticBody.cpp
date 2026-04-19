#include "dnpch.h"
#include "StaticBody.h"

std::shared_ptr<duin::StaticBody> duin::StaticBody::Create(duin::Transform3D transform, duin::CollisionShape collisionShape)
{
    return std::make_shared<duin::StaticBody>(transform, collisionShape);
}

std::shared_ptr<duin::StaticBody> duin::StaticBody::Create(duin::Transform3D transform, duin::CollisionGeometryVariant geometry,
                                                           duin::PhysicsMaterial material)
{
    return std::make_shared<duin::StaticBody>(transform, geometry, material);
}

duin::StaticBody::StaticBody(duin::Transform3D transform, duin::CollisionShape collisionShape)
    : collisionShape(collisionShape)
{
}

duin::StaticBody::StaticBody(duin::Transform3D transform, duin::CollisionGeometryVariant geometry, duin::PhysicsMaterial material)
    : collisionShape(geometry, material)
{
}

duin::StaticBody::~StaticBody()
{
}

duin::Vector3 duin::StaticBody::GetPosition()
{
    return {};
}
