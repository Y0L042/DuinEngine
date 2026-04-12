#include "Module_Doctest.h"

#include <daScript/ast/ast_typefactory_bind.h>
#include <daScript/ast/ast_interop.h>

#include <functional>
#include <vector>
#include <string>

#include "Duin/Core/Debug/DNLog.h"

#include "dn_doctest.das.inc"

// ─── Assertion wrappers ──────────────────────────────────────────────────────
// Each wrapper forwards a runtime bool to doctest's internal decomp_assert,
// using the daslang call-site LineInfoArg for file/line attribution.

static const char *location_file(das::LineInfoArg *at)
{
    return (at && at->fileInfo && at->fileInfo->name.size() > 0)
               ? at->fileInfo->name.c_str()
               : "?";
}

static int location_line(das::LineInfoArg *at)
{
    return at ? (int)at->line : 0;
}

static void dn_check(bool cond, das::LineInfoArg *at)
{
    doctest::detail::decomp_assert(doctest::assertType::DT_CHECK,
                                   location_file(at), location_line(at), "",
                                   doctest::detail::Result(cond));
}

static void dn_check_false(bool cond, das::LineInfoArg *at)
{
    doctest::detail::decomp_assert(doctest::assertType::DT_CHECK_FALSE,
                                   location_file(at), location_line(at), "",
                                   doctest::detail::Result(!cond));
}

static void dn_require(bool cond, das::LineInfoArg *at)
{
    doctest::detail::decomp_assert(doctest::assertType::DT_REQUIRE,
                                   location_file(at), location_line(at), "",
                                   doctest::detail::Result(cond));
}

static void dn_require_false(bool cond, das::LineInfoArg *at)
{
    doctest::detail::decomp_assert(doctest::assertType::DT_REQUIRE_FALSE,
                                   location_file(at), location_line(at), "",
                                   doctest::detail::Result(!cond));
}

static void dn_fail(const char *msg, das::LineInfoArg *at)
{
    doctest::detail::MessageBuilder mb(location_file(at), location_line(at),
                                       doctest::assertType::DT_REQUIRE);
    mb * (msg ? msg : "");
    if (mb.log())
        mb.react();
}

static void dn_fail_check(const char *msg, das::LineInfoArg *at)
{
    doctest::detail::MessageBuilder mb(location_file(at), location_line(at),
                                       doctest::assertType::DT_CHECK);
    mb * (msg ? msg : "");
    if (mb.log())
        mb.react();
}

static void dn_message(const char *msg, das::LineInfoArg *at)
{
    doctest::detail::MessageBuilder mb(location_file(at), location_line(at),
                                       doctest::assertType::DT_WARN);
    mb * (msg ? msg : "");
    mb.log();
}

// ─── Test discovery ──────────────────────────────────────────────────────────
// daslang scripts call dn_register_test(suite, name, fn_name, at) during their
// module-level initialisation (or in game_ready / any early call).  We store
// the context + SimFunction* in a static entry and register a plain trampoline
// with doctest so the test appears in normal test discovery and is run by the
// doctest runner like any C++ TEST_CASE.

struct DasTestEntry
{
    std::string suite;
    std::string name;
    std::string file;
    int line = 0;
    std::shared_ptr<das::Context> context;
    das::SimFunction *fn = nullptr;
};

static std::vector<DasTestEntry> g_dasTests;

// Each registered test needs a unique static void(*)() trampoline.  We
// generate them on demand from a fixed-size pool; increase DAS_TEST_POOL_SIZE
// if you need more than this many daslang tests per process.
static constexpr int DAS_TEST_POOL_SIZE = 256;

static int g_trampolineCount = 0;
static int g_trampolineIndex[DAS_TEST_POOL_SIZE]; // maps trampoline slot → g_dasTests index

template <int N>
static void das_test_trampoline()
{
    const DasTestEntry &e = g_dasTests[g_trampolineIndex[N]];
    e.context->evalWithCatch(e.fn, nullptr, nullptr);
    if (auto ex = e.context->getException())
    {
        doctest::detail::MessageBuilder mb(e.file.c_str(), e.line,
                                           doctest::assertType::DT_REQUIRE);
        mb * ex;
        if (mb.log())
            mb.react();
    }
}

// Build a compile-time table of trampoline function pointers.
template <int... Ns>
static doctest::detail::funcType trampoline_for(int n, std::integer_sequence<int, Ns...>)
{
    static doctest::detail::funcType table[] = {&das_test_trampoline<Ns>...};
    return table[n];
}

static doctest::detail::funcType get_trampoline(int slot)
{
    return trampoline_for(slot, std::make_integer_sequence<int, DAS_TEST_POOL_SIZE>{});
}

// Called from daslang: dn_register_test(suite, name, fn_name, at)
static void dn_register_test(const char *suite, const char *name, const char *fn_name,
                              das::LineInfoArg *at, das::Context *ctx)
{
    if (g_trampolineCount >= DAS_TEST_POOL_SIZE)
    {
        DN_CORE_ERROR("dn_register_test: pool exhausted (max {})", DAS_TEST_POOL_SIZE);
        return;
    }

    das::SimFunction *fn = ctx->findFunction(fn_name ? fn_name : "");
    if (!fn)
    {
        DN_CORE_ERROR("dn_register_test: function '{}' not found in script", fn_name ? fn_name : "");
        return;
    }

    int slot = g_trampolineCount++;
    g_trampolineIndex[slot] = (int)g_dasTests.size();

    DasTestEntry entry;
    entry.suite = suite ? suite : "";
    entry.name = name ? name : "";
    entry.file = location_file(at);
    entry.line = location_line(at);
    // Share ownership of the context so it stays alive while tests run.
    entry.context = std::shared_ptr<das::Context>(ctx, [](das::Context *) {});
    entry.fn = fn;
    g_dasTests.push_back(std::move(entry));

    doctest::detail::TestSuite ts;
    ts.m_test_suite = g_dasTests.back().suite.c_str();

    doctest::detail::TestCase tc(
        get_trampoline(slot),
        g_dasTests.back().file.c_str(),
        (unsigned)g_dasTests.back().line,
        ts);
    tc * g_dasTests.back().name.c_str();

    doctest::detail::regTest(tc);
}

// ─── Module ──────────────────────────────────────────────────────────────────

class Module_Doctest : public das::Module
{
    bool initialized = false;

  public:
    Module_Doctest() : das::Module("dn_doctest")
    {
    }

    bool initDependencies() override
    {
        if (initialized)
            return true;
        initialized = true;

        das::ModuleLibrary lib(this);
        lib.addBuiltInModule();

        das::addExtern<DAS_BIND_FUN(dn_check)>(*this, lib, "dn_check",
            das::SideEffects::modifyExternal, "dn_check")
                ->args({"cond", "at"});

        das::addExtern<DAS_BIND_FUN(dn_check_false)>(*this, lib, "dn_check_false",
            das::SideEffects::modifyExternal, "dn_check_false")
                ->args({"cond", "at"});

        das::addExtern<DAS_BIND_FUN(dn_require)>(*this, lib, "dn_require",
            das::SideEffects::modifyExternal, "dn_require")
                ->args({"cond", "at"});

        das::addExtern<DAS_BIND_FUN(dn_require_false)>(*this, lib, "dn_require_false",
            das::SideEffects::modifyExternal, "dn_require_false")
                ->args({"cond", "at"});

        das::addExtern<DAS_BIND_FUN(dn_fail)>(*this, lib, "dn_fail",
            das::SideEffects::modifyExternal, "dn_fail")
                ->args({"msg", "at"});

        das::addExtern<DAS_BIND_FUN(dn_fail_check)>(*this, lib, "dn_fail_check",
            das::SideEffects::modifyExternal, "dn_fail_check")
                ->args({"msg", "at"});

        das::addExtern<DAS_BIND_FUN(dn_message)>(*this, lib, "dn_message",
            das::SideEffects::modifyExternal, "dn_message")
                ->args({"msg", "at"});

        das::addExtern<DAS_BIND_FUN(dn_register_test)>(*this, lib, "dn_register_test",
            das::SideEffects::modifyExternal, "dn_register_test")
                ->args({"suite", "name", "fn_name", "at", "context"});

        compileBuiltinModule("dn_doctest.das", dn_doctest_das, sizeof(dn_doctest_das));

        DN_CORE_INFO("Script Module [dn_doctest] initialized.");

        return true;
    }
};

REGISTER_MODULE(Module_Doctest);
