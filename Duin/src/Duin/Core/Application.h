#ifndef APPLICATION_H
#define APPLICATION_H

#include <string>
#include <functional>

#include "Duin/Core/Core.h"
#include "Duin/Core/Events/Event.h"



namespace duin
{
    std::string GetRootDirectory();
    void DebugPauseGame();
    void DebugResumeGame();
    int DebugIsGamePaused();

    // void SetActiveCamera3D(::Camera3D camera3d);
    // void SetBackgroundColor(::Color color);

    void SetFramerate(int framerate);
    void DrawPhysicsFPS(float x, float y);
    void DrawRenderFPS(float x, float y);
	float GetPhysicsFPS();
	float GetPhysicsFrameTime();
	float GetRenderFPS();
	float GetRenderFrameTime();
	size_t GetPhysicsFrameCount();
	size_t GetRenderFrameCount();

	void QueuePostUpdateCallback(std::function<void(double)>);
	void QueuePostPhysicsUpdateCallback(std::function<void(double)>);
	void QueuePostDrawCallback(std::function<void()>);
	void QueuePostDrawUICallback(std::function<void()>);
	void QueuePreFrameCallback(std::function<void()>);
	void QueuePostFrameCallback(std::function<void()>);
	void QueuePostDebugCallback(std::function<void()>);

	class DAPI Application
	{
	public:
		Application();
		virtual ~Application();


        void SetWindowStartupSize(int width, int height);
		// void SetBackgroundColor(::Color color);
		void SetWindowName(const char *string);


		void Run();

		void EngineInitialize();
		virtual void Initialize();

		void EngineReady();
		virtual void Ready();

		void EnginePreFrame();

        /** Run as callback events */
		void EngineHandleInputs(InputEvent event);
		virtual void HandleInputs(InputEvent event);

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

		void EngineDebug();
		virtual void Debug();
        void EnginePostDebug();

        void EnginePostFrame();

		virtual void Exit();
		void EngineExit();

	private:
		std::string windowName = "Game";
	};

	Application* CreateApplication();

}

#endif /* APPLICATION_H */
