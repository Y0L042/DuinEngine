#pragma once

#include "PhysX_PhysicsBody.h"
#include "Duin/Physics/StaticBody.h"
#include "Duin/Physics/PhysicsMaterial.h"

namespace duin {
    class PhysXStaticBody
        : public PhysXPhysicsBody, public virtual StaticBody
    {
        public:
            PhysXStaticBody();
            PhysXStaticBody(PhysicsMaterial material);
            ~PhysXStaticBody();

            void SetCollisionShape() override;

        private:
            physx::PxRigidStatic *pxActor = NULL;
            physx::PxMaterial *pxMaterial = NULL;
    };
}
