#pragma once
#include "dnpch.h"

#include "Duin/Core/Core.h"
#include "Duin/Window.h"

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

		void EngineReady();
		virtual void Ready();
		void EngineHandleEvents();
		virtual void HandleEvents();
		void EngineProcess(double rDelta);
		virtual void Process(double rDelta);
		void EnginePhysicsProcess(double pDelta);
		virtual void PhysicsProcess(double pDelta);
		void EngineDraw();
		virtual void Draw();


	private:
		int TARGET_RENDER_FRAMERATE = 60;
		int TARGET_PHYSICS_FRAMERATE = 60;
		std::unique_ptr<Window> win;
	};

	Application* CreateApplication();

}

