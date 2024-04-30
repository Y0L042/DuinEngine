#pragma once
#include "dnpch.h"

#include "Core.h"
#include "Log.h"
#include "Graphics/TextureLoader.h"

#include <SDL.h>
#include <SDL_video.h>

namespace Duin
{
	class Window
	{
	public:
		Window(int screenWidth, int screenHeight);
		Window();
		~Window();

		SDL_Window* GetWindow() { return gWindow; }
		SDL_Surface* GetScreenSurface() { return gScreenSurface; }
		SDL_Renderer* GetRenderer() { return gRenderer; }

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
		SDL_Window* gWindow = NULL;
		SDL_Surface* gScreenSurface = NULL;
		SDL_Renderer* gRenderer = NULL;
		int SCREEN_WIDTH = 800;
		int SCREEN_HEIGHT = 400;

	};
}
