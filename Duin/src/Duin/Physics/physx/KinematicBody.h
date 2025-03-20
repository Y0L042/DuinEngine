#pragma once

#include "PhysicsObject.h"
#include "CollisionShape.h"
#include "PhysicsStructs.h"

#include "Duin/Core/Maths/DuinMaths.h"

#include <memory>

#include <PxPhysicsAPI.h>

namespace duin {
    class KinematicBody : public PhysicsObject
    {
        public:
            static std::shared_ptr<KinematicBody> Create(Transform3D transform, CollisionShape collisionShape);

            KinematicBody(Transform3D transform, CollisionShape collisionShape);
            ~KinematicBody();

            Vector3 GetPosition() override;

        private:
            CollisionShape collisionShape;
            physx::PxRigidDynamic *actor = nullptr;
    };
}
