#pragma once

#include "PhysicsObject.h"
#include "CollisionShape.h"

#include "Duin/Core/Maths/DuinMaths.h"

#include <PxPhysicsAPI.h>

namespace duin {
    class StaticBody : public PhysicsObject
    {
        public:
            StaticBody(Vector3 position, CollisionShape collisionShape);
            ~StaticBody();

        private:
            CollisionShape collisionShape;
            physx::PxRigidStatic *actor = nullptr;
    };
}
