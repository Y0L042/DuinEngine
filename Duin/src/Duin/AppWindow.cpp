#include "dnpch.h"

#include "AppWindow.h"

namespace Duin
{
	AppWindow::AppWindow(int screenWidth, int screenHeight)
		: SCREEN_WIDTH(screenWidth), SCREEN_HEIGHT(screenHeight)
	{
		window = std::make_unique<raylib::Window>(screenWidth, screenHeight, "");
		rlImGuiSetup(true);
	}

	AppWindow::AppWindow()
	{
	}

	AppWindow::~AppWindow()
	{

	}

	bool AppWindow::Init()
	{
		bool initSuccess = true;

		return initSuccess;
	}

	AppWindow& AppWindow::Render()
	{
		UpdateWindow();
		return *this;
	}

	void AppWindow::Close()
	{
		rlImGuiEnd();
		EndDrawing();
	}

	AppWindow& AppWindow::ClearWindow(int r, int g, int b)
	{
		ClearBackground(raylib::Color(r, g, b));
		return *this;
	}
	AppWindow& AppWindow::UpdateWindow()
	{
		BeginDrawing();
		rlImGuiBegin();
		return *this;
	}
	AppWindow& AppWindow::FillWindow(int r, int g, int b)
	{
		return *this;
	}

	AppWindow& AppWindow::SetTargetAppFPS(int fps)
	{
		SetTargetFPS(fps);
		return *this;
	}

	bool AppWindow::ShouldClose()
	{
		return window->ShouldClose();
		//return false;
	}

	AppWindow& AppWindow::Shutdown()
	{
		rlImGuiShutdown();
		return *this;
	}
}