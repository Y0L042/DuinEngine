#pragma once

#include <memory>
#include <string>
#include <Duin/Render/RHI.h>

namespace duin
{
struct Model
{
    
};

enum class ModelType
{
    AUTO, // Uses path .ext to determine if obj or gltf
    OBJ,
    GLTF
};

class ModelLoader
{
  public:
    // Loads model from file into memory
    std::shared_ptr<Model> LoadModel(const std::string &path, ModelType type = ModelType::AUTO);

  private:
};
} // namespace duin