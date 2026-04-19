#pragma once

#include <Jolt/Jolt.h>
#include "Duin/Core/Maths/DuinMaths.h"

namespace duin
{
inline Vector3 FromJPHVec3(JPH::Vec3 vec)
{
    return Vector3(vec.GetX(), vec.GetY(), vec.GetZ());
}

inline JPH::Vec3 ToJPHVec3(Vector3 vec)
{
    return JPH::Vec3(vec.x, vec.y, vec.z);
}
} // namespace duin