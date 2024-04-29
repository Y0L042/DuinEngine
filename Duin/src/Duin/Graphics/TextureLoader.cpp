#include "dnpch.h"

#include "TextureLoader.h"

namespace Duin
{
	TextureLoader& TextureLoader::SetRenderer(SDL_Renderer* renderer)
	{
		TextureLoader& instance = GetInstance();
		instance.gRenderer = renderer;
		return instance;
	}
}