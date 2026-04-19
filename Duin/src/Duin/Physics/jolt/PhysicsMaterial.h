#pragma once

#include <rfl.hpp>

namespace duin
{

class PhysicsMaterial
{
  public:
    float staticFriction;
    float dynamicFriction;
    float restitutionCoeff;

    PhysicsMaterial(float staticFriction = 1.0f, float dynamicFriction = 1.0f, float restitutionCoeff = 1.0f);

    using ReflectionType = struct
    {
        float staticFriction, dynamicFriction, restitutionCoeff;
    };
    PhysicsMaterial(const ReflectionType &impl)
        : PhysicsMaterial(impl.staticFriction, impl.dynamicFriction, impl.restitutionCoeff)
    {
    }
    const ReflectionType &reflection() const
    {
        return ReflectionType{staticFriction, dynamicFriction, restitutionCoeff};
    }

  private:
    friend class CollisionShape;
};

} // namespace duin
