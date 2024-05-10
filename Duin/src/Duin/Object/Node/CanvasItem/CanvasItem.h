#pragma once

#include "Duin/Core/Core.h"
#include "Duin/Object/Node/Node.h"

namespace Duin
{
	class DUIN_API CanvasItem : public Node
	{
	public:
		CanvasItem();
		~CanvasItem();

		virtual void Draw();

	private:

	};
}