#include "dnpch.h"

#include "TextureRes.h"

namespace Duin
{
	TextureRes::TextureRes()
	{
	}

	TextureRes::TextureRes(const char* texturePath)
	{
		LoadTexture(texturePath);
	}

	TextureRes::TextureRes(const char* texturePath, float width, float height)
	{
		LoadTexture(texturePath);
		SetTextureSize(width, height);
	}

	TextureRes::TextureRes(const char* texturePath, raylib::Vector2 size)
	{
		LoadTexture(texturePath);
		SetTextureSize(size);
	}

	TextureRes::~TextureRes()
	{

	}

	TextureRes& TextureRes::LoadTexture(const char* texturePath)
	{
		Texture texture = ::LoadTexture(texturePath); // supposed to call Raylib's LoadTexture()
		// Assign the loaded texture to the shared_ptr with a custom deleter
		texturePtr.reset(new Texture(texture), [](Texture* ptr) {
			UnloadTexture(*ptr);  // Custom deleter that calls Raylib's UnloadTexture
			delete ptr;           // Delete the pointer after unloading the texture
			});
		return *this;
	}

	TextureRes& TextureRes::SetTextureSize(float width, float height)
	{
		textureSize = raylib::Vector2(width, height);
		SetRLTextureSize((int)width, (int)height);
		return *this;
	}

	TextureRes& TextureRes::SetTextureSize(raylib::Vector2 size)
	{
		textureSize = size;
		SetRLTextureSize((int)size.x, (int)size.y);
		return *this;
	}

	TextureRes& TextureRes::Draw(float posX, float posY)
	{
		::DrawTexture(*(texturePtr), (int)posX, (int)posY, { 255, 255, 255, 255 });
		return *this;
	}

	TextureRes& TextureRes::Draw(raylib::Vector2 position)
	{
		::DrawTexture(*(texturePtr.get()), (int)position.x, (int)position.y, { 255, 255, 255, 255 });
		return *this;
	}

	void TextureRes::ClearTexture()
	{

	}

	void TextureRes::SetRLTextureSize(int width, int height)
	{
		texturePtr->width = width;
		texturePtr->height = height;
	}
}