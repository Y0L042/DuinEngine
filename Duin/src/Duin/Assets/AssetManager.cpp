#include "dnpch.h"
#include "AssetManager.h"
#include "FileTypes.h"
#include "Duin/IO/Filesystem.h"
#include <memory>

static duin::fs::EnumerationResult DetectAssetCallback(void *userdata, const char *dirname, const char *fname)
{
    if (userdata != nullptr && fname != nullptr)
    {
        duin::FileExtension fe = duin::GetPathInfo(fname).fileInfo;
        std::shared_ptr<duin::Asset> asset = std::make_shared<duin::Asset>();
        asset->archeType = duin::FS_ARCHETYPE_FILE;
        asset->fileType = fe.type;
        asset->fileExt = fe.ext;
        asset->path = dirname;

        std::unordered_map<duin::UUID, std::shared_ptr<duin::Asset>>& assetMap =
            *(std::unordered_map<duin::UUID, std::shared_ptr<duin::Asset>> *)userdata;
        assetMap[asset->uuid] = asset;
    }
    return duin::fs::DNFS_ENUM_CONTINUE;
}

duin::AssetManager::AssetManager() : basePath(fs::GetBasePath())
{
}

duin::AssetManager::~AssetManager()
{
}

duin::UUID duin::AssetManager::GetUUID()
{
    return uuid;
}
bool duin::AssetManager::CatalogueAssets()
{
    bool result = duin::fs::EnumerateDirectory(basePath.c_str(), DetectAssetCallback, &assetMap);

    for (auto& asset : assetMap)
    {
        DN_CORE_INFO("Asset {} {}", asset.first.ToStrHex(), asset.second->path);
    }

    return result;
}
duin::AssetManager &duin::AssetManager::Get()
{
    static duin::AssetManager am;
    return am;
}
