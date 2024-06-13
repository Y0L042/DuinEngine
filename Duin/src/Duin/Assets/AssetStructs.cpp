#include "dnpch.h"

#include "AssetStructs.h"
#include "AssetManager.h"

namespace Duin
{
	std::shared_ptr<_Asset> _TextureAssetFactory::CreateAsset(AssetManager* assetManager, const char* path)
	{
		// TODO
		::Texture rlTexture = ::LoadTexture(path);
		std::shared_ptr<_TextureAsset> textureAsset = std::make_shared<_TextureAsset>(rlTexture);

		return textureAsset;
	}

	std::shared_ptr<_Asset> _ImageAssetFactory::CreateAsset(AssetManager* assetManager, const char* path)
	{
		// TODO
		::Image rlImage = ::LoadImage(path);
		std::shared_ptr<_ImageAsset> imageAsset = std::make_shared<_ImageAsset>(rlImage);

		return imageAsset;
	}
}