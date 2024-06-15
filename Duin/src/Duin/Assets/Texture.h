#pragma once

#include "Duin/Core/Core.h"
#include "Duin/Assets/AssetStructs.h"
#include "Duin/Core/Structures/RenderStructs.h"
#include "Duin/Assets/Asset.h"
#include "Duin/Assets/GUID.h"
#include "Duin/Core/Maths/DuinMaths.h"

#include <memory>

namespace Duin
{
	class AssetManager;

	/*
	* Wrapper class for the _TextureAsset struct.
	* The client will interact with the textures through this class.
	*/
	class DUIN_API Texture : public Asset
	{
	public:
		Texture();
		Texture(AssetManager* assetManager, const char* path);
		~Texture();

		Vector2 GetTextureSize();
		void SetTextureSize(Vector2 size);
		void ResetTextureSize();

		void Draw(Vector2 position, float rotation_deg = 1.0f, Color color = Duin::WHITE, float scale = 1.0f, bool centered = true);

	private:
		std::shared_ptr<_TextureAsset> _textureAsset;
		
		Vector2 _defaultSize;
		Vector2 size;
	};
}