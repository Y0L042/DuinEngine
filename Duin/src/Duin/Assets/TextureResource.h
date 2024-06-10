#pragma once
#include "dnpch.h"

#include "Duin/Core/Core.h"
#include "Duin/Core/Debug/Log.h"
#include "Duin/Core/Maths/DuinMaths.h"
#include "Duin/Core/Structures/RenderStructs.h"

#include <RayGuiComponent.h>

namespace Duin
{
	class DUIN_API TextureResource
	{
	public:
		TextureResource();
		TextureResource(const char* texturePath);
		TextureResource(const char* texturePath, float width, float height);
		TextureResource(const char* texturePath, Vector2 size);
		~TextureResource();


		TextureResource& LoadTexture(const char* texturePath);
		TextureResource& SetTextureSize(float width, float height);
		TextureResource& SetTextureSize(Vector2 size);

		TextureResource& Draw(float posX, float posY, float rotation_deg);
		TextureResource& Draw(Vector2 position, float rotation_deg);

		TextureResource& SetCentered(bool centered);
		TextureResource& SetTintColor(Color color);

		void ClearTexture();

	private:
		std::shared_ptr<::Texture> texturePtr;
		Vector2 textureSize;
		bool isCentered = false;
		Color tintColor = WHITE;
	};
} 