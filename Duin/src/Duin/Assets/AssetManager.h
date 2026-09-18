/*
* The goal of the AssetManager is to store references to raw asset data, and only hand out 
* references to the assets. This is to prevent duplicate-loading the same assets by accident.
* There are two stages for model loading:
*   1. Import
*       The model is given a file containing a UUID, and import settings
*   2. Loading
*       Imported models are loaded into memory
*/

#pragma once
#include "Duin/Core/Utils/UUID.h"
#include <assimp/Importer.hpp>
#include <assimp/scene.h>
#include <assimp/postprocess.h>
#include "Asset.h"
#include "Model.h"
#include <unordered_map>
#include <optional>

namespace duin
{


class AssetManager
{
  public:
    static AssetManager &Get();

    AssetManager();
    ~AssetManager();

    UUID GetUUID();
    std::optional<Model> LoadModel(const std::string &path);

    bool HasAsset(const std::string& path);
    // Generate and share meshes

  private:
    UUID uuid;
    std::string basePath;
    Assimp::Importer importer;
    std::unordered_map<std::string, std::shared_ptr<duin::Asset>> assetMap;

    std::shared_ptr<ModelData> LoadMesh(const std::string& path);

};

} // namespace duin
