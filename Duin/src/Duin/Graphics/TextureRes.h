#pragma once
#include "dnpch.h"

#include "Duin/Core/Core.h"
#include "Duin/Core/Debug/Log.h"
#include "Duin/Graphics/TextureLoader.h"

#include <RayGuiComponent.h>

namespace Duin
{
	class DUIN_API TextureRes
	{
	public:
		TextureRes();
		TextureRes(const char* texturePath);
		TextureRes(const char* texturePath, float width, float height);
		TextureRes(const char* texturePath, raylib::Vector2 size);
		~TextureRes();


		TextureRes& LoadTexture(const char* texturePath);
		TextureRes& SetTextureSize(float width, float height);
		TextureRes& SetTextureSize(raylib::Vector2 size);

		TextureRes& Draw(float posX, float posY);
		TextureRes& Draw(raylib::Vector2 position);

		void ClearTexture();

	private:
		std::shared_ptr<::Texture> texturePtr;
		raylib::Vector2 textureSize;
	};
} 