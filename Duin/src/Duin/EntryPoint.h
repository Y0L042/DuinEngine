#ifndef ENTRYPOINT_H
#define ENTRYPOINT_H

#ifdef DN_PLATFORM_WINDOWS

#include "Duin/Core/Debug/DNLog.h"
#include "Duin/Core/Debug/DNAssert.h"
#include "Duin/ECS/DnFlecs.h"
#include <daScript/misc/crash_handler.h>

extern duin::Application *duin::CreateApplication(int argc, char **argv);

int main(int argc, char **argv)
{
    duin::Log::Init();
    DN_CORE_WARN("Initialized Log.");
    DN_INFO("Initialized Log.");

    duin::InitAsserts();
    DN_CORE_WARN("Initialized Asserts.");
    DN_INFO("Initialized Asserts.");

    duin::SetFlecsDaslangAssert();
    DN_CORE_WARN("Installed flecs assert hook.");
    DN_INFO("Installed flecs assert hook.");

    das::install_das_crash_handler();
    DN_CORE_WARN("Installed daslang crash handler.");
    DN_INFO("Installed daslang crash handler.");

    auto app = duin::CreateApplication(argc, argv);
    app->Run();
    delete app;

    return 0;
}

#endif

#endif /* ENTRYPOINT_H */
