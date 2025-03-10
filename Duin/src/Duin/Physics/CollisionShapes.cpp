#include "dnpch.h"
#include "CollisionShapes.h"

// Use PhysX by default
#ifndef JOLT
#define PHYSICS_CSHAPE PhysXCollisionShape
#include "./impl/physx/PhysX_Includes.h"
#endif

#ifdef JOLT
#ifndef PHYSX
#define PHYSICS_CSHAPE
#endif
#endif

namespace duin {
    std::shared_ptr<CollisionShape> CollisionShape::Create()
    {
        // std::shared_ptr<PHYSICS_CSHAPE> shape = std::make_shared<PHYSICS_CSHAPE>();
        // return shape;
        return PhysicsServer::Get().CreateCollisionShape();
    }

    UUID CollisionShape::GetUUID()
    {
        return uuid;
    }
}
