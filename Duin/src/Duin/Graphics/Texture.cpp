#include "dnpch.h"

#include "Texture.h"

namespace Duin
{
	Texture::Texture(std::string& texturePath)
	{
		LoadTexture(texturePath);
	}

	Texture::Texture(const char* texturePath)
	{
		LoadTexture(texturePath);
	}

	Texture::Texture()
	{
	}

	Texture::~Texture()
	{

	}

	Texture& Texture::LoadTexture(const char* texturePath)
	{
		ClearTexture();


		return *this;
	}

	Texture& Texture::LoadTexture(std::string& texturePath)
	{
		return LoadTexture(texturePath.c_str());
	}

	Texture& Texture::Draw(float posX, float posY, float width, float height)
	{

		return *this;
	}

	void Texture::ClearTexture()
	{

	}
}