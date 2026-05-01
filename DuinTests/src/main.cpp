// #define DOCTEST_CONFIG_IMPLEMENT_WITH_MAIN
// DOCTEST_CONFIG_IMPLEMENT is now defined in Duin/src/Duin/Core/Debug/DNAssert.cpp
// so the doctest runtime lives in Duin.lib. Do not redefine it here.
#define DN_DISABLE_ALL_LOGGING
#include <doctest.h>
#include <iostream>
#include <ctime>
#include <sstream>
#include <corecrt.h>

#include <daScript/daScript.h>
#include <Duin/Script/Script.h>
#include <Duin/Script/GameScript.h>
#include <Duin/Script/ScriptModules.h>

#include <filesystem>

#define __TESTING__

const char testLogDir[] = "./TestLogs";

enum class RunCtx
{
    ALL,
    CPP,
    DAS
};

void RunScriptTests();

std::string getTimestampedFilename()
{
    std::time_t now = std::time(nullptr);
    std::tm *local = std::localtime(&now);
    std::ostringstream oss;
    oss << testLogDir << "/test_log_";
    oss << (1900 + local->tm_year) << "-" << (1 + local->tm_mon) << "-" << local->tm_mday << "_" << local->tm_hour
        << "-" << local->tm_min << "-" << local->tm_sec << ".log";
    return oss.str();
}

int main(int argc, char **argv)
{
    int res = 0;

    //RunCtx runCtx = RunCtx::ALL;
     RunCtx runCtx = RunCtx::DAS;

    if (argc > 1)
    {
        const char *runContext = argv[1];
        if (strcmp(runContext, "CPP") == 0)
        {
            runCtx = RunCtx::CPP;
        }
        else if (strcmp(runContext, "DAS") == 0)
        {
            runCtx = RunCtx::DAS;
        }
        else if (strcmp(runContext, "ALL") == 0)
        {
            runCtx = RunCtx::ALL;
        }
    }

    if (runCtx == RunCtx::ALL || runCtx == RunCtx::CPP)
    {
        doctest::Context ctx;
        ctx.setOption("abort-after", 50);
        ctx.setOption("no-breaks", true);
        ctx.applyCommandLine(argc, argv);

        // Run the tests
        res = ctx.run();

        if (ctx.shouldExit())
        {
            return res;
        }
    }

    if (runCtx == RunCtx::ALL || runCtx == RunCtx::DAS)
    {
        RunScriptTests();
    }

    return res;
}

void RunScriptTests()
{
    namespace fs = std::filesystem;
    
    int passed = 0, failed = 0;

    duin::Script script;
    script.SetDasRoot("C:\\Projects\\CPP_Projects\\Duin\\Duin\\vendor\\daslang");
    script.SetProjectFile("C:\\Projects\\CPP_Projects\\Duin\\DuinTests\\duintests.das_project");
    script.InitModules([]() {
        NEED_MODULE(Module_flecs);
        NEED_MODULE(Module_imgui);

        NEED_MODULE(Module_DnLog);
        NEED_MODULE(Module_DnRenderer);
        NEED_MODULE(Module_DnCamera);
        NEED_MODULE(Module_DnGameObject);
        NEED_MODULE(Module_DnECS);
        NEED_MODULE(Module_DecsGameWorld);
        NEED_MODULE(Module_DnInput);
        NEED_MODULE(Module_DnPhysicsServer);
        NEED_MODULE(Module_DnCharacterBody);
        NEED_MODULE(Module_DnApplication);
    });

    for (const auto &entry : fs::recursive_directory_iterator("./scripts")) {
        if (entry.path().extension() != ".das") continue;

        std::string path = entry.path().generic_string();
        std::cout << "Running script: " << path << "\n";

        script.SetScriptPath(path);
        bool ok = script.Compile() && script.SimulateContext();
        std::cout << (ok ? "  PASSED\n" : "  FAILED\n");
        ok ? ++passed : ++failed;
    }

    std::cout << "\nScript tests: " << passed << " passed, " << failed << " failed\n";
}
