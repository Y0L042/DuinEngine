#include "dnpch.h"

#include "AssetManager.h"

namespace Duin
{
	AssetManager::AssetManager()
	{
	}

	AssetManager::~AssetManager()
	{
	}

	void AssetManager::AddToAssetRegistry(std::shared_ptr<_Asset> asset)
	{
		assetRegistry[asset->guid] = asset;
	}

	void AssetManager::RemoveFromAssetRegistry(GUID guid)
	{
		assetRegistry.erase(guid);
	}
}