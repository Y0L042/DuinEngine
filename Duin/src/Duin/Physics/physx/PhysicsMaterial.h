#pragma once

#include <PxPhysicsAPI.h>

namespace duin {
    class PhysicsMaterial
    {
        public:
            float staticFriction;
            float dynamicFriction;
            float restitutionCoeff;

            PhysicsMaterial(float staticFriction = 1.0f,
                            float dynamicFriction = 1.0f,
                            float restitutionCoeff = 1.0f);

            physx::PxMaterial *pxMaterial = nullptr;
        private:
            class CollisionShape;
            friend class CollisionShape;
            friend class PhysicsMaterial;

    };
}
