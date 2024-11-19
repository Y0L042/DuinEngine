#ifndef APPLICATION_H
#define APPLICATION_H

#include <string>
#include <functional>

#include "Duin/Core/Core.h"
#include <dep_raygui.h>



namespace duin
{

    void SetActiveCamera3D(::Camera3D camera3d);
    void SetBackgroundColor(::Color color);

	void QueuePostUpdateCallback(std::function<void(double)>);
	void QueuePostPhysicsUpdateCallback(std::function<void(double)>);
	void QueuePostDrawCallback(std::function<void()>);
	void QueuePostDrawUICallback(std::function<void()>);

	class DAPI Application
	{
	public:
		Application();
		virtual ~Application();

		void SetFramerate(int framerate)
		{
			TARGET_RENDER_FRAMERATE = framerate;
			TARGET_PHYSICS_FRAMERATE = framerate;
		}
        void SetWindowStartupSize(int width, int height);
		void SetBackgroundColor(::Color color);
		void SetWindowName(const char *string);


		void Run();

		void EngineInitialize();
		virtual void Initialize();

		void EngineReady();
		virtual void Ready();

		void EngineHandleInputs();
		virtual void HandleInputs();

		void EngineUpdate(double delta);
		virtual void Update(double delta);
        void EnginePostUpdate(double delta);

		void EnginePhysicsUpdate(double delta);
		virtual void PhysicsUpdate(double delta);
        void EnginePostPhysicsUpdate(double delta);

		void EngineDraw();
		virtual void Draw();
        void EnginePostDraw();

		void EngineDrawUI();
		virtual void DrawUI();
        void EnginePostDrawUI();

        void EnginePostFrame();

		virtual void Exit();
		void EngineExit();

	private:
		int TARGET_RENDER_FRAMERATE = 60;
		int TARGET_PHYSICS_FRAMERATE = 60;
		std::string windowName = "Game";
	};

	Application* CreateApplication();

}

#endif /* APPLICATION_H */
