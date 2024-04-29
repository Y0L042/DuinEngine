#include "dnpch.h"

#include "Texture.h"

namespace Duin
{
	Texture::Texture(std::string texturePath)
		: texture(nullptr, DestroyTexture)
	{
		LoadTexture(texturePath);
	}

	Texture::Texture()
		: texture(nullptr, DestroyTexture)
	{
	}

	Texture::~Texture()
	{
		if (texture)
		{
			SDL_DestroyTexture(texture.get());
		}
		texture = nullptr;
	}

	Texture& Texture::LoadTexture(const char* texturePath)
	{
		ClearTexture();

		SDL_Renderer* renderer = TextureLoader::GetRenderer();
		if (!renderer)
		{
			DN_CORE_ERROR("Renderer not found! \n");
			return *this;
		}

		SDL_Surface* image = SDL_LoadBMP(texturePath);
		if (!image)
		{
			DN_CORE_FATAL("Unable to load image {}! SDL Error: {} \n", texturePath, SDL_GetError());
			return *this;
		}
		SDL_Texture* rawTexture = SDL_CreateTextureFromSurface(renderer, image);
		if (!rawTexture)
		{
			DN_CORE_ERROR("Texture failed to create! SDL Error: {} \n", SDL_GetError());
			return *this;
		}

		if (image)
		{
			SDL_FreeSurface(image);
		}

		texture.reset(rawTexture);
		DN_CORE_INFO("Texture loaded: {}", texturePath);

		return *this;
	}

	Texture& Texture::LoadTexture(std::string& texturePath)
	{
		return LoadTexture(texturePath.c_str());
	}

	void Texture::ClearTexture()
	{
		if (texture)
		{
			SDL_DestroyTexture(texture.get());
			texture = nullptr;
		}
	}
}