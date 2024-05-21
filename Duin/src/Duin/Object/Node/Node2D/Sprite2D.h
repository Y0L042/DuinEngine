#pragma once

#include "Duin/Core/Core.h"
#include "Duin/Core/Maths/DuinMaths.h"
#include "Duin/Object/Node/Node2D/Node2D.h"
#include "Duin/Graphics/TextureRes.h"

#include <string>
#include <memory>

namespace Duin
{
	class DUIN_API Sprite2D : public Node2D
	{
	public:
		Sprite2D();
		Sprite2D(const char* texturePath);
		~Sprite2D();

		std::shared_ptr<TextureRes> GetTexture();
		std::shared_ptr<Sprite2D> SetTexture(char* texturePath);
		std::shared_ptr<Sprite2D> SetTexture(TextureRes* texturePtr);
		std::shared_ptr<Sprite2D> SetTexture(std::shared_ptr<TextureRes> texturePtr);

		std::shared_ptr<Sprite2D> SetTextureSize(float width, float height);
		std::shared_ptr<Sprite2D> SetTextureSize(Vector2 size);

		void Draw() override;

	private:
		std::shared_ptr<TextureRes> texture;
	};
}