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

	}

	bool Window::Init()
	{
		bool initSuccess = true;

		return initSuccess;
	}

	Window& Window::Render()
	{
		UpdateRenderer();
		return *this;
	}

	void Window::Close()
	{
	}
	Window& Window::ClearRenderer()
	{
		return *this;
	}
	Window& Window::UpdateRenderer()
	{
		return *this;
	}
	Window& Window::FillWindow(int r, int g, int b)
	{
		return *this;
	}
}