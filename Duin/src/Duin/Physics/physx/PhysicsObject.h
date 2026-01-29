#pragma once

#include "CollisionObject.h"

#include "Duin/Core/Maths/DuinMaths.h"

namespace duin
{
class PhysicsObject : public CollisionObject
{
  public:
    virtual Vector3 GetPosition() = 0;

  private:
};
} // namespace duin
