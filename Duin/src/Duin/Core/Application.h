#ifndef APPLICATION_H
#define APPLICATION_H

#include <string>

#include "Duin/core/core.h"
#include "Duin/dep_raygui.h"


namespace Duin
{
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

        void SetActiveCamera2D(::Camera2D *camera2d);
        void SetActiveCamera3D(::Camera3D *camera3d);

		void Run();

		void EngineInitialize();
		virtual void Initialize();
		void EngineReady();
		virtual void Ready();
		void EngineHandleInputs();
		virtual void HandleInputs();
		void EngineUpdate(double rDelta);
		virtual void Update(double rDelta);
		void EnginePhysicsUpdate(double pDelta);
		virtual void PhysicsUpdate(double pDelta);
		void EngineDraw();
		virtual void Draw();
		void EngineDrawUI();
		virtual void DrawUI();

		virtual void Exit();
		void EngineExit();

	private:
		int TARGET_RENDER_FRAMERATE = 60;
		int TARGET_PHYSICS_FRAMERATE = 60;
		Color backgroundColor = WHITE;
		std::string windowName = "Game";
	};

	Application* CreateApplication();

}

#endif /* APPLICATION_H */
