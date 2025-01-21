#ifndef ENTRYPOINT_H
#define ENTRYPOINT_H

#ifdef DN_PLATFORM_WINDOWS

extern duin::Application* duin::CreateApplication();

int main(int argc, char** argv)
{
	duin::Log::Init();
	DN_CORE_WARN("Initialized Log.");
	DN_INFO("Initialized Log.");

    auto app = duin::CreateApplication();
	app->Run();
	delete app;

	return 0;
}

#endif

#endif /* ENTRYPOINT_H */
