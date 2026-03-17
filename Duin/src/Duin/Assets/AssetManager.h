#pragma once
#include "Duin/Core/Utils/UUID.h"

#include "Asset.h"
#include <unordered_map>

namespace duin
{
class AssetManager
{
  public:
    static AssetManager &Get();

    AssetManager();
    ~AssetManager();

    UUID GetUUID();
    bool CatalogueAssets();

    // Generate and share meshes

  private:
    UUID uuid;
    std::string basePath;
    std::unordered_map<duin::UUID, std::shared_ptr<duin::Asset>> assetMap;

};

} // namespace duin
