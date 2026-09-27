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
    std::string path;
    Vector3 baseScale = Vector3(1.0f, 1.0f, 1.0f);
    std::shared_ptr<ModelData> data;

    Model()
    {
    }

    Model(const std::string &path) : path(path)
    {
    }

    void Draw(Vector3 position, Quaternion rotation, Vector3 scale = Vector3(1.0f, 1.0f, 1.0f)) const;
};

} // namespace duin