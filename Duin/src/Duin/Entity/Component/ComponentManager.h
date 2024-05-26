#pragma once

#include "Duin/Core/Core.h"

namespace Duin
{
	class DUIN_API ComponentManager
	{
	public:
		virtual void Update() = 0;
	};
}