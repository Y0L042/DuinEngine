#pragma once
#include "dnpch.h"

#include "Core.h"
#include "Log.h"

#include <SDL.h>
#include <SDL_video.h>


class Window
{
public:
	Window(int screenWidth, int screenHeight);
	Window();
	~Window();

	SDL_Window* GetWindow() { return gWindow; }
	SDL_Surface* GetScreenSurface() { return gScreenSurface; }

	void SetScreenSize(int screenWidth, int screenHeight)
	{
		SCREEN_WIDTH = screenWidth;
		SCREEN_HEIGHT = screenHeight;
	}

	bool Init();
	bool LoadMedia();
	void Render();
	void Close();


private:
	SDL_Window* gWindow = NULL;
	SDL_Surface* gScreenSurface = NULL;
	int SCREEN_WIDTH = 800;
	int SCREEN_HEIGHT = 400;

};
