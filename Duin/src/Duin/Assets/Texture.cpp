#include "dnpch.h"

#include "Texture.h"
#include "Duin/Assets/AssetManager.h"


namespace Duin
{
	Texture::Texture()
	{
	}

	Texture::Texture(AssetManager* assetManager, const char* path)
		: Asset(assetManager)
	{
		_textureAsset = assetManager->LoadAsset<_TextureAsset, _TextureAssetFactory>(path);
	}

	Texture::~Texture()
	{
	}

	Vector2 Texture::GetTextureSize()
	{
		return Vector2{ (int)_textureAsset->width, (int)_textureAsset->height };
	}

	void Texture::SetCentered(bool centered)
	{
		this->centered = centered;
	}

	void Texture::Draw(Vector2 position, float rotation)
	{
		// TODO
	}
}