#include "dnpch.h"

#include "Window.h"

Window::Window(int screenWidth, int screenHeight)
	: SCREEN_WIDTH(screenWidth), SCREEN_HEIGHT(screenHeight)
{
}

Window::Window()
{
}

Window::~Window()
{
}

bool Window::Init()
{
	bool initSuccess = true;

	if (SDL_Init(SDL_INIT_VIDEO) < 0)
	{
		DN_CORE_FATAL("SDL could not initialize! SDL_Error: %s\n", SDL_GetError());
		initSuccess = false;
	}
	else
	{
		gWindow = SDL_CreateWindow("SDL Tutorial", SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, SCREEN_WIDTH, SCREEN_HEIGHT, SDL_WINDOW_SHOWN);
		if (gWindow == NULL)
		{
			DN_CORE_FATAL("Window could not be created! SDL_Error: %s\n", SDL_GetError());
			initSuccess = false;
		}
		else
		{
			gScreenSurface = SDL_GetWindowSurface(gWindow);
		}
	}

	return initSuccess;
}

bool Window::LoadMedia()
{
	return false;
}

void Window::Render()
{
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
