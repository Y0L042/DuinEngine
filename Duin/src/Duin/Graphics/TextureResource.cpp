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
		SetTextureSize(texturePtr->width, texturePtr->height);
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

	TextureResource& TextureResource::Draw(float posX, float posY, float rotation_deg)
	{
		if (!texturePtr) 
		{ 
			DN_CORE_WARN("TextureRes texture pointer not set!");
			return *this;
		}

		Draw(Vector2{ posX, posY }, rotation_deg);
		return *this;
	}

	TextureResource& TextureResource::Draw(Vector2 position, float rotation_deg)
	{
		if (!texturePtr)
		{
			DN_CORE_WARN("TextureRes texture pointer not set!");
			return *this;
		}
		
		Vector2 origin = Vector2::ZERO;
		if (isCentered)
		{
			position -= textureSize / 2;
			origin += textureSize / 2;
		}

		::DrawTexturePro
		(
			*(texturePtr),
			{ 0, 0, (float)texturePtr->width, (float)texturePtr->height },
			{ position.x, position.y, textureSize.x, textureSize.y },
			{ origin.x, origin.y },
			rotation_deg,
			{ 255, 255, 255, 255 }
		);
		//std::cout << "Drawing texture with size: " << textureSize.x << ", " << textureSize.y << "\n";
		return *this;
	}

	TextureResource& TextureResource::SetCentered(bool centered)
	{
		isCentered = centered;
		return *this;
	}

	void TextureResource::ClearTexture()
	{
		texturePtr.reset();
	}
}