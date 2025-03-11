#pragma once

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

        private:
    };
}
