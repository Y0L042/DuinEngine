#pragma once

#include "Duin/Core/Core.h"
#include "Duin/Core/Maths/DuinMaths.h"
#include "Duin/Object/Node/CanvasItem/Node2D/Node2D.h"
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
		Sprite2D& SetTexture(char* texturePath);
		Sprite2D& SetTexture(TextureRes* texturePtr);
		Sprite2D& SetTexture(std::shared_ptr<TextureRes> texturePtr);

		void Ready() override;
		void Draw() override;

	private:
		const char* texturePath;
		std::shared_ptr<TextureRes> texture;
		raylib::Vector2 textureScale;

	};
}