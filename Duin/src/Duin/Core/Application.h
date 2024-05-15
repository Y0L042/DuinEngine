#pragma once
#include "dnpch.h"

#include "Duin/Core/Core.h"
#include "Duin/Core/AppWindow.h"
#include "Duin/Events/InputEvent.h"
#include "Duin/Events/InputMap.h"
#include "Duin/Object/ObjectManager.h"

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

		template<typename T, typename... Args>
		std::shared_ptr<T> InstantiateChild(Args&&... args)
		{
			std::shared_ptr<T> objPtr = ObjectManager::GetRootNode().InstantiateChild(std::forward<Args>(args)...);
			return objPtr;
		}
		void AddChild(std::shared_ptr<Object> child);
		void RemoveChild(std::shared_ptr<Object> child);

		void Run();

		void EngineInitialize();
		virtual void Initialize();
		void EngineReady();
		virtual void Ready();
		void EngineHandleInputs(InputEvent e);
		virtual void HandleInputs(InputEvent e);
		void EngineProcess(double rDelta);
		virtual void Process(double rDelta);
		void EnginePhysicsProcess(double pDelta);
		virtual void PhysicsProcess(double pDelta);
		void EngineDraw();
		virtual void Draw();
		void EngineExit();
		virtual void Exit();

	private:
		int TARGET_RENDER_FRAMERATE = 60;
		int TARGET_PHYSICS_FRAMERATE = 60;
		std::unique_ptr<AppWindow> win;
	};

	Application* CreateApplication();

}

