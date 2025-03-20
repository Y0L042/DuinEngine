#pragma once

#include "PhysicsObject.h"
#include "CollisionShape.h"
#include "PhysicsStructs.h"

#include "Duin/Core/Maths/DuinMaths.h"

#include <memory>

#include <PxPhysicsAPI.h>

namespace duin {
    class StaticBody : public PhysicsObject
    {
        public:
            static std::shared_ptr<StaticBody> Create(Transform3D transform, CollisionShape collisionShape);

            StaticBody(Transform3D transform, CollisionShape collisionShape);
            ~StaticBody();

            Vector3 GetPosition() override;

        private:
            CollisionShape collisionShape;
            physx::PxRigidStatic *actor = nullptr;
    };
}
