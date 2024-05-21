#include "dnpch.h"

#include "AppWindow.h"

namespace Duin
{
	AppWindow::AppWindow(int screenWidth, int screenHeight)
		: SCREEN_WIDTH(screenWidth), SCREEN_HEIGHT(screenHeight)
	{
		::InitWindow(screenWidth, screenHeight, "");
		::rlImGuiSetup(true);
	}

	AppWindow::AppWindow()
	{
	}

	AppWindow::~AppWindow()
	{
		CloseWindow();
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
		::CloseWindow();
	}

	AppWindow& AppWindow::ClearWindow(int r, int g, int b)
	{
		::ClearBackground(Color{ static_cast<unsigned char>(r), static_cast<unsigned char>(g), static_cast<unsigned char>(b), 255 });
		return *this;
	}
	AppWindow& AppWindow::UpdateWindow()
	{
		::BeginDrawing();
		::rlImGuiBegin();
		return *this;
	}
	AppWindow& AppWindow::EndUpdateWindow()
	{
		::rlImGuiEnd();
		::EndDrawing();
		return *this;
	}
	AppWindow& AppWindow::FillWindow(int r, int g, int b)
	{
		return *this;
	}

	AppWindow& AppWindow::SetTargetAppFPS(int fps)
	{
		::SetTargetFPS(fps);
		return *this;
	}

	bool AppWindow::ShouldClose()
	{
		return ::WindowShouldClose();
	}

	AppWindow& AppWindow::Shutdown()
	{
		::rlImGuiShutdown();
		return *this;
	}
	double AppWindow::GetFrametime()
	{
		return ::GetFrameTime();
	}
}