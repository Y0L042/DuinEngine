#pragma once
#include "Duin/Core/Maths/DuinMaths.h"
#include <daScript/daScript.h>

namespace duin
{
inline das::float3 to_f3(const Vector3 &v)
{
    return {v.x, v.y, v.z};
}

inline duin::Vector3 from_f3(const das::float3 &f3)
{
    return {f3.x, f3.y, f3.z};
}

inline das::float4 to_f4(const Vector4 &v)
{
    return {v.x, v.y, v.z, v.w};
}

inline duin::Vector4 from_f4(const das::float4 &f4)
{
    return {f4.x, f4.y, f4.z, f4.w};
}
} // namespace duin