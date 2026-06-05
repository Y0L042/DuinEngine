// This file MUST NOT use the precompiled header (see premake5.lua filter).
// The defines below must precede the first inclusion of doctest.h; MSVC's PCH
// mechanism would otherwise ignore them.
#ifndef DOCTEST_CONFIG_DISABLE
// Provide the doctest runtime in Duin.lib so consumers don't have to.
#define DOCTEST_CONFIG_IMPLEMENT
#define DOCTEST_CONFIG_SUPER_FAST_ASSERTS
#endif

#include <doctest.h>
#include "Duin/Core/Debug/DNAssert.h"
#include <iostream>

static std::function<std::string()> assertContextCallback;

static void assert_handler(const doctest::AssertData &ad)
{
    using namespace doctest;

    // uncomment if asserts will be used in a multi-threaded context
    // std::lock_guard<std::mutex> lock(g_mut);

    // here we can choose what to do:
    // - log the failed assert
    // - throw an exception
    // - call std::abort() or std::terminate()

    std::cout << Color::LightGrey << skipPathFromFilename(ad.m_file) << "(" << ad.m_line << "): ";
    std::cout << Color::Red << failureString(ad.m_at) << ": ";

    // handling only normal (comparison and unary) asserts - exceptions-related asserts have been skipped
    if (ad.m_at & assertType::is_normal)
    {
        std::cout << Color::Cyan << assertString(ad.m_at) << "( " << ad.m_expr << " ) ";
        std::cout << Color::None << (ad.m_threw ? "THREW exception: " : "is NOT correct!\n");
        if (ad.m_threw)
            std::cout << ad.m_exception;
        else
            std::cout << "  values: " << assertString(ad.m_at) << "( " << ad.m_decomp << " )";
    }
    else
    {
        std::cout << Color::None << "an assert dealing with exceptions has failed!";
    }

    if (assertContextCallback)
    {
        std::string extra = assertContextCallback();
        if (!extra.empty())
        {
            std::cout << "\n[Script callstack]\n" << extra << "\n";
        }
    }

    std::cout << std::endl;

}

static doctest::Context* g_assertContext = nullptr;

void duin::InitAsserts()
{
    // Must outlive all assert calls; heap-allocated so it is never destroyed.
    g_assertContext = new doctest::Context();

    // sets the context as the default one - so asserts used outside of a testing context do not crash
    g_assertContext->setAsDefaultForAssertsOutOfTestCases();

    // set a handler with a signature: void(const doctest::AssertData&)
    // without setting a handler we would get std::abort() called when an assert fails
    g_assertContext->setAssertHandler(assert_handler);
}

void duin::SetAssertContextCallback(std::function<std::string()> callback)
{
    assertContextCallback = std::move(callback);
}

void duin::OnCrash(const char *message)
{
    std::cout << doctest::Color::Red << "[CRASH] " << (message ? message : "(no message)") << "\n";
    if (assertContextCallback) {
        std::string stack = assertContextCallback();
        if (!stack.empty())
            std::cout << doctest::Color::None << "[Script callstack]\n" << stack << "\n";
    }
    std::abort();
}
