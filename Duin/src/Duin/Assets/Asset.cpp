#include "dnpch.h"

#include "Asset.h"
#include "Duin/Assets/AssetManager.h"

namespace Duin
{
	Asset::Asset()
	{
	}

	Asset::Asset(AssetManager* assetManager)
		: assetManager(assetManager)
	{
	}

	Asset::~Asset()
	{
	}

	void Asset::SetAssetManager(AssetManager* assetManager)
	{
		this->assetManager = assetManager;
	}

	AssetManager* Asset::GetAssetManager()
	{
		return assetManager;
	}
}