#pragma once

#include "Duin/Core/Maths/DuinMaths.h"

// Currently just a grouping class for physics objects

namespace duin
{

class PhysicsObject
{
  public:
    virtual Vector3 GetPosition() = 0;
};

} // namespace duin
