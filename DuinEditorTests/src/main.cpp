// DOCTEST_CONFIG_IMPLEMENT is defined in Duin/src/Duin/Core/Debug/DNAssert.cpp
// so the doctest runtime lives in Duin.lib. Do not redefine it here.
#define DN_DISABLE_ALL_LOGGING
#include <doctest.h>
#include <iostream>
#include <ctime>
#include <sstream>
#include <corecrt.h>

int main(int argc, char **argv)
{
    doctest::Context ctx;
    ctx.setOption("abort-after", 50);
    ctx.setOption("no-breaks", true);
    ctx.applyCommandLine(argc, argv);

    int res = ctx.run();

    if (ctx.shouldExit())
        return res;

    return res;
}
