#pragma once

#include "Duin/Core/Maths/DuinMaths.h"

namespace duin
{

typedef struct Transform3D
{
    Vector3 translation;
    Quaternion rotation;
} Transform3D;

} // namespace duin
