#pragma once
#include "dnpch.h"

#include "Duin/Core/Core.h"
#include "Duin/Core/Debug/Log.h"
#include "Duin/Graphics/TextureLoader.h"

#include <RayGuiComponent.h>

namespace Duin
{
	class AppWindow
	{
	public:
		AppWindow(int screenWidth, int screenHeight);
		AppWindow();
		~AppWindow();



		void SetScreenSize(int screenWidth, int screenHeight)
		{
			SCREEN_WIDTH = screenWidth;
			SCREEN_HEIGHT = screenHeight;
		}

		bool Init();
		AppWindow& Render();
		void Close();

		AppWindow& ClearWindow(int r, int g, int b);
		AppWindow& UpdateWindow();
		AppWindow& FillWindow(int r, int g, int b);
		AppWindow& SetTargetAppFPS(int fps);
		AppWindow& Shutdown();
		double GetFrametime();
		bool ShouldClose();


	private:
		int SCREEN_WIDTH = 800;
		int SCREEN_HEIGHT = 400;
		std::unique_ptr<raylib::Window> window;

	};
}
