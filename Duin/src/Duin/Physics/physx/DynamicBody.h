#pragma once

#include "PhysicsObject.h"
#include "CollisionShape.h"

#include "PhysicsStructs.h"

#include "Duin/Core/Maths/DuinMaths.h"

#include <PxPhysicsAPI.h>

namespace duin {
    class DynamicBody : public PhysicsObject
    {
        public:
            DynamicBody(Transform3D transform, CollisionShape collisionShape);
            ~DynamicBody();

        private:
            CollisionShape collisionShape;
            physx::PxRigidDynamic *actor = nullptr;
    };
}
