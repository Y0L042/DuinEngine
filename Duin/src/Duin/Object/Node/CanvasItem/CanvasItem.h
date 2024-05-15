#pragma once

#include "Duin/Core/Core.h"
#include "Duin/Object/Node/Node.h"

#include <functional>

namespace Duin
{
	class DUIN_API CanvasItem : public Node
	{
	public:
		CanvasItem();
		~CanvasItem();

		virtual void Draw() override;

	private:

	};
}