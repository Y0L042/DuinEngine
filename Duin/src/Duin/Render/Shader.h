#pragma once

#include "RHI.h"

namespace duin
{
inline RHIShaderHandle LoadShader(const char *path)
{
    return RHILoadShader(path);
}
} // namespace duin
