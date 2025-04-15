#pragma once

#include "PhysicsObject.h"
#include "CollisionShape.h"
#include "PhysicsStructs.h"

#include "Duin/Core/Maths/DuinMaths.h"

#include <memory>

#include <PxPhysicsAPI.h>

namespace duin {
    class DynamicBody : public PhysicsObject
    {
        public:
            static std::shared_ptr<DynamicBody> Create(Transform3D transform, CollisionShape collisionShape);
            static std::shared_ptr<DynamicBody> Create(Transform3D transform, CollisionGeometryVariant geometry, PhysicsMaterial material);

            DynamicBody(Transform3D transform, CollisionShape collisionShape);
            DynamicBody(Transform3D transform, CollisionGeometryVariant geometry, PhysicsMaterial material);
            ~DynamicBody();

            Vector3 GetPosition() override;
            Quaternion GetRotation();

        private:
            CollisionShape collisionShape;
            physx::PxRigidDynamic *actor = nullptr;
    };
}
