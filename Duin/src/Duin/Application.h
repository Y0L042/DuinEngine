#pragma once

#include "Core.h"
#include "raylib-cpp.hpp"

namespace Duin
{
	class DUIN_API Application
	{
	public:
		Application();
		virtual ~Application();

		void Run();
	};

	Application* CreateApplication();
}

