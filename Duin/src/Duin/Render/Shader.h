#pragma once

#include <bgfx/bgfx.h>
#include <cstring>
#include <bgfx/platform.h>

namespace duin
{
bgfx::ShaderHandle LoadShader(const char *FILENAME);
}
