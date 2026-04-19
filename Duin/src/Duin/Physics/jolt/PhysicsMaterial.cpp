#include "dnpch.h"
#include "PhysicsMaterial.h"

duin::PhysicsMaterial::PhysicsMaterial(float staticFriction, float dynamicFriction, float restitutionCoeff)
    : staticFriction(staticFriction), dynamicFriction(dynamicFriction), restitutionCoeff(restitutionCoeff)
{
}
