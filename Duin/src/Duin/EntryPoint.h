#pragma once

#ifdef DN_PLATFORM_WINDOWS

extern Duin::Application* Duin::CreateApplication();

int main(int argc, char** argv)
{
	Duin::Log::Init();
	DN_CORE_WARN("Initialized Log!");
	DN_INFO("Initialized Log.");

	auto app = Duin::CreateApplication();
	app->Run();
	delete app;
}

#endif