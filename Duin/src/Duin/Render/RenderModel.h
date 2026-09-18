#pragma once

#include "RHI.h"

namespace duin
{

struct RenderModel
{
    RHIVertexLayout vertexLayout;
    size_t bufferSize;
    RHIVertexBufferHandle vertexBufferHandle;
    RHIIndexBufferHandle indexBufferHandle;
    RHIShaderHandle shaderHandle;
};

} // namespace duin