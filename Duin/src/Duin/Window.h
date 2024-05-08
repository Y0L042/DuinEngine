#pragma once
#include "dnpch.h"

#include "Duin/Core/Core.h"
#include "Duin/Core/Debug/Log.h"
#include "Duin/Graphics/TextureLoader.h"

#include "imgui.h"
#include "rlImGui.h"

namespace Duin
{
	class Window
	{
	public:
		Window(int screenWidth, int screenHeight);
		Window();
		~Window();



		void SetScreenSize(int screenWidth, int screenHeight)
		{
			SCREEN_WIDTH = screenWidth;
			SCREEN_HEIGHT = screenHeight;
		}

		bool Init();
		Window& Render();
		void Close();

		Window& ClearWindow(int r, int g, int b);
		Window& UpdateWindow();
		Window& FillWindow(int r, int g, int b);
		bool ShouldClose();


	private:
		int SCREEN_WIDTH = 800;
		int SCREEN_HEIGHT = 400;
		std::unique_ptr<raylib::Window> window;

	};
}
