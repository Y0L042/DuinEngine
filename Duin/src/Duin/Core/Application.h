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
    void DrawFPS(bool enable);
    void DrawPhysicsFPS(float x, float y);
    void DrawRenderFPS(float x, float y);
	float GetPhysicsFPS();
	float GetPhysicsFrameTime();
	float GetRenderFPS();
	float GetRenderFrameTime();
	size_t GetPhysicsFrameCount();
	size_t GetRenderFrameCount();
    double GetGameTime();
    double GetGameTimeMilli();
    double GetGameTimeNano();
    void GameDelay(double milliSeconds);

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

		virtual void Initialize();
		virtual void Ready();
        /** Run as callback events */
		virtual void HandleInputs(InputEvent event);
        /* ----------------------- */
        virtual void Update(double delta);
        virtual void PhysicsUpdate(double delta);
        virtual void Draw();
        virtual void DrawUI();
        virtual void Debug();
        virtual void Exit();

    private:
        std::string windowName = "Game";

		void EngineInitialize();

		void EngineReady();

		void EnginePreFrame();

		void EngineHandleInputs(InputEvent event);

        void EngineUpdate(double delta);
        void EnginePostUpdate(double delta);

        void EnginePhysicsUpdate(double delta);
        void EnginePostPhysicsUpdate(double delta);

        void EngineDraw();
        void EnginePostDraw();

        void EngineDrawUI();
        void EnginePostDrawUI();
        void EnginePostFrame();

        void EngineDebug();
        void EnginePostDebug();

        void EngineExit();
    };

    Application* CreateApplication();

}

#endif /* APPLICATION_H */
