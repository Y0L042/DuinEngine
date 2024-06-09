#pragma once
#include "dnpch.h"

#include "Duin/Core/Core.h"
#include "Duin/Events/InputMap.h"
#include "Duin/Events/InputEvent.h"
#include "Duin/Core/Scene/SceneManager.h"

#include <RayGuiComponent.h>


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

		void EngineInitialize();
		virtual void Initialize();
		void EngineReady();
		virtual void Ready();
		void EngineHandleInputs(InputEvent e);
		virtual void HandleInputs(InputEvent e);
		void EngineUpdate(double rDelta);
		virtual void Update(double rDelta);
		void EnginePhysicsUpdate(double pDelta);
		virtual void PhysicsUpdate(double pDelta);
		void EngineDraw();
		virtual void Draw();
		void EngineExit();
		virtual void Exit();

		SceneManager& GetSceneManager();

	private:
		int TARGET_RENDER_FRAMERATE = 60;
		int TARGET_PHYSICS_FRAMERATE = 60;
		SceneManager sceneManager;
	};

	Application* CreateApplication();

}

