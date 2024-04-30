#pragma once
#include "dnpch.h"

#include "Duin/Core/Core.h"
#include "Duin/Core/Debug/Log.h"
#include <SDL.h>
#include "TextureLoader.h"


namespace Duin
{
	class DUIN_API Texture
	{
	public:
		Texture(std::string& texturePath);
		Texture(const char* texturePath);
		Texture();
		~Texture();

		Texture& LoadTexture(const char* texturePath);
		Texture& LoadTexture(std::string& texturePath);

		Texture& Draw(float posX, float posY, float width, float height);

		void ClearTexture();

	private:
		SDL_Texture* texture = nullptr;
	};
} 