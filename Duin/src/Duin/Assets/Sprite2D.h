#pragma once

#include "Duin/Core/Core.h"
#include "Duin/Core/Scene/Object.h"
#include "Duin/Assets/Texture.h"
#include "Duin/Core/Maths/DuinMaths.h"

namespace Duin
{
	class DUIN_API Sprite2D : public Object
	{
	public:
		Sprite2D();
		Sprite2D(std::shared_ptr<Texture> texture);
		~Sprite2D();

		void SetTexture(std::shared_ptr<Texture> texture);
		std::shared_ptr<Texture> GetTexture();

	private:
		std::shared_ptr<Texture> texture;
		Transform2D transform;
	};
}