#pragma once
#include "dnpch.h"

#include "Duin/Core/Core.h"
#include "Duin/Core/Debug/Log.h"
#include "Duin/Graphics/TextureLoader.h"

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

		Window& ClearRenderer();
		Window& UpdateRenderer();
		Window& FillWindow(int r, int g, int b);


	private:
		int SCREEN_WIDTH = 800;
		int SCREEN_HEIGHT = 400;

	};
}
