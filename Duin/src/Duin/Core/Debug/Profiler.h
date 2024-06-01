#pragma once

#include "Duin/Core/Core.h"

#include <RLImGuiComponent.h>

namespace Duin
{
	class DUIN_API Profiler
	{
	public:
		Profiler();
		~Profiler();

		void Draw();

	private:
		bool enabled = true;
	};
}