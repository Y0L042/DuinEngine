#include "daScript/daScript.h"

#include <iostream>
#include <windows.h>

using namespace das;

void tutorial()
{
    TextPrinter tout;
    ModuleGroup dummyLibGroup;
    auto fAccess = make_smart<FsFileAccess>();

    auto program = compileDaScript("scripts/hello.das", fAccess, tout, dummyLibGroup);
    if (program->failed())
    {
        tout << "Compilation failed:\n";
        for (auto &err : program->errors)
        {
            tout << reportError(err.at, err.what, err.extra, err.fixme, err.cerr);
        }
        return;
    }

    Context ctx(program->getContextStackSize());
    if (!program->simulate(ctx, tout))
    {
        tout << "Simulation failed:\n";
        for (auto &err : program->errors)
        {
            tout << reportError(err.at, err.what, err.extra, err.fixme, err.cerr);
        }
        return;
    }

    auto fnTest = ctx.findFunction("test");
    if (!fnTest)
    {
        tout << "Function 'test' not found\n";
        return;
    }

    if (!verifyCall<void>(fnTest->debugInfo, dummyLibGroup))
    {
        tout << "Function 'test' has wrong signature; expected def test() : void\n";
        return;
    }

    ctx.evalWithCatch(fnTest, nullptr);
    if (auto ex = ctx.getException())
    {
        tout << "Script exception: " << ex << "\n";
    }
}

int main(int, char *[])
{
    NEED_ALL_DEFAULT_MODULES;
    Module::Initialize();

    size_t x = 100000;
    while (x-- > 0)
    {
        tutorial();
        Sleep(1000);
    }

    Module::Shutdown();
    return 0;
}
