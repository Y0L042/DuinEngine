#include "dnpch.h"
#include "DynamicBody.h"

std::shared_ptr<duin::DynamicBody> duin::DynamicBody::Create(duin::Transform3D transform, duin::CollisionShape collisionShape)
{
    return std::make_shared<duin::DynamicBody>(transform, collisionShape);
}

std::shared_ptr<duin::DynamicBody> duin::DynamicBody::Create(duin::Transform3D transform, duin::CollisionGeometryVariant geometry,
                                                             duin::PhysicsMaterial material)
{
    return std::make_shared<duin::DynamicBody>(transform, geometry, material);
}

duin::DynamicBody::DynamicBody(duin::Transform3D transform, duin::CollisionShape collisionShape)
    : collisionShape(collisionShape)
{
}

duin::DynamicBody::DynamicBody(duin::Transform3D transform, duin::CollisionGeometryVariant geometry, duin::PhysicsMaterial material)
    : collisionShape(geometry, material)
{
}

duin::DynamicBody::~DynamicBody()
{
}

duin::Vector3 duin::DynamicBody::GetPosition()
{
    return {};
}

duin::Quaternion duin::DynamicBody::GetRotation()
{
    return {};
}
