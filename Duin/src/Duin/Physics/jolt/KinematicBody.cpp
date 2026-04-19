#include "dnpch.h"
#include "KinematicBody.h"

std::shared_ptr<duin::KinematicBody> duin::KinematicBody::Create(duin::Transform3D transform, duin::CollisionShape collisionShape)
{
    return std::make_shared<duin::KinematicBody>(transform, collisionShape);
}

std::shared_ptr<duin::KinematicBody> duin::KinematicBody::Create(duin::Transform3D transform, duin::CollisionGeometryVariant geometry,
                                                                  duin::PhysicsMaterial material)
{
    return std::make_shared<duin::KinematicBody>(transform, geometry, material);
}

duin::KinematicBody::KinematicBody(duin::Transform3D transform, duin::CollisionShape collisionShape)
    : collisionShape(collisionShape)
{
}

duin::KinematicBody::KinematicBody(duin::Transform3D transform, duin::CollisionGeometryVariant geometry, duin::PhysicsMaterial material)
    : collisionShape(geometry, material)
{
}

duin::KinematicBody::~KinematicBody()
{
}

duin::Vector3 duin::KinematicBody::GetPosition()
{
    return {};
}
