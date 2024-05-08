#include "dnpch.h"

#include "Window.h"

namespace Duin
{
	Window::Window(int screenWidth, int screenHeight)
		: SCREEN_WIDTH(screenWidth), SCREEN_HEIGHT(screenHeight)
	{
		window = std::make_unique<raylib::Window>(screenWidth, screenHeight, "");
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
		UpdateWindow();
		return *this;
	}

	void Window::Close()
	{
		rlImGuiEnd();
		EndDrawing();
	}

	Window& Window::ClearWindow(int r, int g, int b)
	{
		ClearBackground(raylib::Color(r, g, b));
		return *this;
	}
	Window& Window::UpdateWindow()
	{
		BeginDrawing();
		rlImGuiBegin();
		return *this;
	}
	Window& Window::FillWindow(int r, int g, int b)
	{
		return *this;
	}
	bool Window::ShouldClose()
	{
		return window->ShouldClose();
		//return false;
	}
}