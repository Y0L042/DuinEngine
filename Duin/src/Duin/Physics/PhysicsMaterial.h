#pragma once

namespace duin {
    struct IPhysicsMaterial
    {
        public:
            float staticFriction;
            float dynamicFriction;
            float restitutionCoeff;
        private:
    };
}
