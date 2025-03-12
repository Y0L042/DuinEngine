#pragma once

#include <PxPhysicsAPI.h>

namespace duin {
    class PhysicsMaterial
    {
        public:
            float staticFriction;
            float dynamicFriction;
            float restitutionCoeff;

            PhysicsMaterial(float staticFriction,
                            float dynamicFriction,
                            float restitutionCoeff);

            physx::PxMaterial *pxMaterial = nullptr;
        private:
            class CollisionShape;
            friend class CollisionShape;
            friend class PhysicsMaterial;

    };
}
