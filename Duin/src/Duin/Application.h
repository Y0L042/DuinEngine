#pragma once
#include "dnpch.h"

#include "Core.h"
#include "SDL.h"

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

