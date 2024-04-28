#pragma once
#include "dnpch.h"

#include "Core.h"
#include "Window.h"

namespace Duin
{
	class DUIN_API Application
	{
	public:
		Application();
		virtual ~Application();

		void SetFramerate(int framerate)
		{
			TARGET_RENDER_FRAMERATE = framerate;
			TARGET_PHYSICS_FRAMERATE = framerate;
		}

		void Run();

		void Ready();
		void HandleEvents();
		void Process(double rDelta);
		void PhysicsProcess(double pDelta);
		void Render();

	private:
		int TARGET_RENDER_FRAMERATE = 60;
		int TARGET_PHYSICS_FRAMERATE = 60;
		std::unique_ptr<Window> win;
	};

	Application* CreateApplication();

}

