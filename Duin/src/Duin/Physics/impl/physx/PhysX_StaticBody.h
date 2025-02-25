#pragma once

#include "PhysX_PhysicsBody.h"
#include "Duin/Physics/StaticBody.h"

namespace duin {
    class PhysXStaticBody
        : public PhysXPhysicsBody, public virtual StaticBody
    {
        public:
            PhysXStaticBody();
            ~PhysXStaticBody();

        private:
            physx::PxRigidStatic *pxActor = NULL;
            physx::PxMaterial *pxMaterial = NULL;
    };
}
