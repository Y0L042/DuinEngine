#include "dnpch.h"

#include "TextureResource.h"

namespace Duin
{
	TextureResource::TextureResource()
	{
	}

	TextureResource::TextureResource(const char* texturePath)
	{
		LoadTexture(texturePath);
	}

	TextureResource::TextureResource(const char* texturePath, float width, float height)
	{
		LoadTexture(texturePath);
		SetTextureSize(width, height);
	}

	TextureResource::TextureResource(const char* texturePath, Vector2 size)
	{
		LoadTexture(texturePath);
		SetTextureSize(size);
	}

	TextureResource::~TextureResource()
	{

	}

	TextureResource& TextureResource::LoadTexture(const char* texturePath)
	{
		Texture texture = ::LoadTexture(texturePath); // supposed to call Raylib's LoadTexture()
		texturePtr.reset(new Texture(texture), [](Texture* ptr) {
			UnloadTexture(*ptr);  // Custom deleter that calls Raylib's UnloadTexture
			delete ptr;           // Delete the pointer after unloading the texture
			});
		return *this;
	}

	TextureResource& TextureResource::SetTextureSize(float width, float height)
	{
		textureSize = Vector2(width, height);
		return *this;
	}

	TextureResource& TextureResource::SetTextureSize(Vector2 size)
	{
		textureSize = size;
		return *this;
	}

	TextureResource& TextureResource::Draw(float posX, float posY)
	{
		if (!texturePtr) 
		{ 
			DN_CORE_WARN("TextureRes texture pointer not set!");
			return *this;
		}

		Draw(Vector2{ posX, posY });
		return *this;
	}

	TextureResource& TextureResource::Draw(Vector2 position)
	{
		if (!texturePtr)
		{
			DN_CORE_WARN("TextureRes texture pointer not set!");
			return *this;
		}

		::DrawTexturePro
		(
			*(texturePtr),
			{ 0, 0, (float)texturePtr->width, (float)texturePtr->height },
			{ position.x, position.y, textureSize.x, textureSize.y },
			{ 0, 0 },
			0,
			{ 255, 255, 255, 255 }
		);
		//std::cout << "Drawing texture with size: " << textureSize.x << ", " << textureSize.y << "\n";
		return *this;
	}

	void TextureResource::ClearTexture()
	{
		texturePtr.reset();
	}
}