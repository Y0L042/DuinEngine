#include "dnpch.h"

#include "Texture.h"

namespace Duin
{
	Texture::Texture(std::string& texturePath)
		: texture(nullptr)
	{
		LoadTexture(texturePath);
	}

	Texture::Texture(const char* texturePath)
		: texture(nullptr)
	{
		LoadTexture(texturePath);
	}

	Texture::Texture()
		: texture(nullptr)
	{
	}

	Texture::~Texture()
	{
		if (texture)
		{
			SDL_DestroyTexture(texture);
		}
		texture = nullptr;
	}

	Texture& Texture::LoadTexture(const char* texturePath)
	{
		ClearTexture();

		SDL_Renderer* renderer = TextureLoader::GetRenderer();
		if (!renderer)
		{
			DN_CORE_ERROR("Renderer not found!");
			return *this;
		}

		SDL_Surface* image = SDL_LoadBMP(texturePath);
		if (!image)
		{
			DN_CORE_FATAL("Unable to load image {}! SDL Error: {}", texturePath, SDL_GetError());
			return *this;
		}
		texture = SDL_CreateTextureFromSurface(renderer, image);
		if (!texture)
		{
			DN_CORE_ERROR("Texture failed to create! SDL Error: {}", SDL_GetError());
			return *this;
		}

		if (image)
		{
			SDL_FreeSurface(image);
		}

		DN_CORE_INFO("Texture loaded: {}", texturePath);

		return *this;
	}

	Texture& Texture::LoadTexture(std::string& texturePath)
	{
		return LoadTexture(texturePath.c_str());
	}

	Texture& Texture::Draw(float posX, float posY, float width, float height)
	{
		SDL_Rect textureRect{posX, posY, width, height};
		SDL_Renderer* renderer = TextureLoader::GetRenderer();
		SDL_RenderCopy(renderer, texture, NULL, &textureRect);

		return *this;
	}

	void Texture::ClearTexture()
	{
		if (texture)
		{
			SDL_DestroyTexture(texture);
			texture = nullptr;
		}
	}
}