#pragma once

#include "Duin/Core/Core.h"
#include "Duin/Events/InputMap.h"
#include "Duin/Events/InputEvent.h"
#include "Duin/Core/Scene/SceneManager.h"
#include "Duin/Core/Structures/RenderStructs.h"
#include "Duin/Core/Debug/DebugDraw.h"

#include <RayGuiComponent.h>

#include <string>

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
		void SetBackgroundColor(Color color);
		void SetWindowName(const char* string);

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
		virtual void Exit();
		void EngineExit();

		SceneManager& GetSceneManager();

	private:
		int TARGET_RENDER_FRAMERATE = 60;
		int TARGET_PHYSICS_FRAMERATE = 60;
		Color backgroundColor = WHITE;
		std::string windowName = "Game";

		SceneManager sceneManager;
	};

	Application* CreateApplication();

}

