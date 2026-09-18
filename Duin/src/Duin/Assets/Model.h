#pragma once

#include <memory>
#include <string>
#include "Asset.h"
#include <Duin/Render/RHI.h>

namespace duin
{
struct ModelData : public Asset
{
    RHIVertexLayout vertexLayout;
    size_t bufferSize;
    RHIVertexBufferHandle vertexBufferHandle;
    RHIIndexBufferHandle indexBufferHandle;
    RHIShaderHandle shaderHandle;

    std::vector<duin::PosColorVertex> vertices;
    std::vector<uint16_t> indices;
};

class Model
{
  public:
    Vector3 baseScale = Vector3(1.0f, 1.0f, 1.0f);
    std::shared_ptr<ModelData> data;
};

} // namespace duin