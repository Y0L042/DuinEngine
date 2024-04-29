#pragma once
#include "dnpch.h"

#include "Duin/Core.h"
#include "Duin/Log.h"
#include <SDL.h>
#include "TextureLoader.h"


namespace Duin
{
	class DUIN_API Texture
	{
	public:
		Texture(std::string texturePath);
		Texture();
		~Texture();

		Texture& LoadTexture(const char* texturePath);
		Texture& LoadTexture(std::string& texturePath);

		void ClearTexture();

	private:
		// Define the smart pointer with a custom deleter directly inside the class
		std::unique_ptr<SDL_Texture, void(*)(SDL_Texture*)> texture;

		// Static method to serve as a deleter
		static void DestroyTexture(SDL_Texture* texture) 
		{
			if (texture) 
			{
				SDL_DestroyTexture(texture);
			}
		}
	};
} 