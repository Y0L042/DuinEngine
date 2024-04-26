#pragma once

#ifdef DN_PLATFORM_WINDOWS

extern Duin::Application* Duin::CreateApplication();

int main(int argc, char** argv)
{
	printf("Starting Duin Engine...");
	auto app = Duin::CreateApplication();
	app->Run();
	delete app;
}

#endif