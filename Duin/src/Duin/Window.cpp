#include "dnpch.h"

#include "Window.h"

namespace Duin
{
	Window::Window(int screenWidth, int screenHeight)
		: SCREEN_WIDTH(screenWidth), SCREEN_HEIGHT(screenHeight)
	{
	}

	Window::Window()
	{
	}

	Window::~Window()
	{
		if (gRenderer)
		{
			SDL_DestroyRenderer(gRenderer);
		}
		gRenderer = NULL;

		if (gWindow)
		{
			SDL_DestroyWindow(gWindow);
		}
		gWindow = NULL;

		if (gScreenSurface)
		{
			SDL_FreeSurface(gScreenSurface);
		}
		gScreenSurface = NULL;

		SDL_Quit();
	}

	bool Window::Init()
	{
		bool initSuccess = true;

		if (SDL_Init(SDL_INIT_VIDEO) < 0)
		{
			DN_CORE_FATAL("SDL could not initialize! SDL_Error: %s \n", SDL_GetError());
			initSuccess = false;
		}
		else
		{
			gWindow = SDL_CreateWindow("SDL Tutorial", SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, SCREEN_WIDTH, SCREEN_HEIGHT, SDL_WINDOW_SHOWN);
			if (gWindow == NULL)
			{
				DN_CORE_FATAL("Window could not be created! SDL_Error: %s \n", SDL_GetError());
				initSuccess = false;
			}
			else
			{
				gScreenSurface = SDL_GetWindowSurface(gWindow);
			}
		}

		gRenderer = SDL_CreateRenderer(gWindow, -1, SDL_RENDERER_ACCELERATED);
		if (gRenderer)
		{
			DN_CORE_INFO("Hardware renderer created. \n");
		}
		else
		{
			//DN_CORE_WARN("Hardware renderer could not be created! Trying software renderer... \n");
			gRenderer = SDL_CreateSoftwareRenderer(gScreenSurface);
			if (!gRenderer)
			{
				DN_CORE_FATAL("Renderer could not be created! SDL_Error: %s \n", SDL_GetError());
				return false;
			}
			DN_CORE_INFO("Software renderer created. \n");
		}
		TextureLoader::SetRenderer(gRenderer);

		SDL_SetRenderDrawColor(gRenderer, 255, 255, 255, 255);
		SDL_RenderClear(gRenderer);
		return true;

		return initSuccess;
	}

	void Window::Render()
	{
		SDL_FillRect(GetScreenSurface(), NULL, SDL_MapRGB(GetScreenSurface()->format, 255, 255, 255));
		SDL_UpdateWindowSurface(GetWindow());
	}

	void Window::Close()
	{
		//Destroy window
		SDL_DestroyWindow(gWindow);
		gWindow = NULL;

		//Quit SDL subsystems
		SDL_Quit();
	}
}