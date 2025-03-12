#pragma once

#include "PhysicsObject.h"
#include "CollisionShape.h"

#include "PhysicsStructs.h"

#include "Duin/Core/Maths/DuinMaths.h"

#include <PxPhysicsAPI.h>

namespace duin {
    class KinematicBody : public PhysicsObject
    {
        public:
            KinematicBody(Transform3D transform, CollisionShape collisionShape);
            ~KinematicBody();

        private:
            CollisionShape collisionShape;
            physx::PxRigidDynamic *actor = nullptr;
    };
}
