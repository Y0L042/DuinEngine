#include "dnpch.h"
#include "PhysicsMaterial.h"

#include "PhysicsServer.h"

namespace duin
{
PhysicsMaterial::PhysicsMaterial(float staticFriction, float dynamicFriction, float restitutionCoeff)
    : staticFriction(staticFriction), dynamicFriction(dynamicFriction), restitutionCoeff(restitutionCoeff),
      pxMaterial(PhysicsServer::Get().pxPhysics->createMaterial(staticFriction, dynamicFriction, restitutionCoeff))
{
}

} // namespace duin
