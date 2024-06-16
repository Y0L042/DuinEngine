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
		_defaultSize = size = Vector2(_textureAsset->width, _textureAsset->height);
	}

	Texture::~Texture()
	{
	}

	Vector2 Texture::GetTextureSize()
	{
		return Vector2{ (float)_textureAsset->width, (float)_textureAsset->height };
	}

	void Texture::SetTextureSize(Vector2 size)
	{
		this->size = size;
	}

	void Texture::SetTextureWidthKeepRatio(float width)
	{
		float ratio = _defaultSize.x / _defaultSize.y;
		size.x = width;
		size.y = width / ratio;
	}

	void Texture::SetTextureHeightKeepRatio(float height)
	{
		float ratio = _defaultSize.x / _defaultSize.y;
		size.y = height;
		size.x = height * ratio;
	}

	void Texture::ResetTextureSize()
	{
		size = _defaultSize;
	}

	void Texture::Draw(Vector2 position, float rotation_deg, Color color, float scale, bool centered)
	{	
		float scaledWidth = size.x * scale;
		float scaledHeight = size.y * scale;
		float halfW = scaledWidth / 2.0f;
		float halfH = scaledHeight / 2.0f;

		::DrawTexturePro(
			_textureAsset->rlTextureCache,
			::Rectangle(0, 0, _textureAsset->width, _textureAsset->height),
			::Rectangle(position.x, position.y, scaledWidth, scaledHeight),
			::Vector2(halfW, halfH),
			rotation_deg,
			::Color(color.r, color.g, color.b, color.a)
		);
	}
}