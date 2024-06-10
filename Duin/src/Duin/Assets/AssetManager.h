#pragma once

#include "Duin/Core/Core.h"
#include "Duin/Core/UUID.h"
#include "Duin/Core/Structures/RenderStructs.h"

#include "RLImGuiComponent.h"

#include <unordered_map>

namespace Duin
{
	class DUIN_API AssetManager
	{
	public:
		AssetManager();
		~AssetManager();

		void LoadTexture(const char* texturePath);

	private:
		std::unordered_map<UUID, std::shared_ptr<Texture>> textureMap;
	};
}