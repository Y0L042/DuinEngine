#pragma once
#include "dnpch.h"

#include "Duin/Core/Core.h"
#include "Duin/Core/UUID.h"
#include "Duin/Core/Maths/DuinMaths.h"
#include "Duin/Object/Node/CanvasItem/CanvasItem.h"

namespace Duin
{
	class DUIN_API Node2D : public CanvasItem
	{
	public:
		Node2D();
		Node2D(raylib::Vector2 globalPosition2D);
		~Node2D();

		raylib::Vector2 GetGlobalPosition2D();
		void SetGlobalPosition2D(raylib::Vector2 pos);

	protected:
		raylib::Vector2 globalPosition2D{};

	private:

	};
}