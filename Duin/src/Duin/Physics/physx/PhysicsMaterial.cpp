#include "dnpch.h"
#include "PhysicsMaterial.h"

namespace duin {
    PhysicsMaterial::PhysicsMaterial(float staticFriction,
                            float dynamicFriction,
                            float restitutionCoeff)
        : staticFriction(staticFriction),
        dynamicFriction(dynamicFriction),
        restitutionCoeff(restitutionCoeff)
    {

    }

}
