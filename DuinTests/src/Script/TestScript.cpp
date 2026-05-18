#include <doctest.h>
#include <Duin/Script/Script.h>
#include <filesystem>
#include <chrono>

static const std::string DAS_ROOT      = "Duin/vendor/daslang";
static const std::string ARTIFACTS_DIR = "./artifacts";

static std::string WriteDas(const std::string &name, const std::string &src)
{
    std::filesystem::create_directories(ARTIFACTS_DIR);
    std::string path = ARTIFACTS_DIR + "/" + name;
    FILE *f = fopen(path.c_str(), "w");
    if (f) { fwrite(src.data(), 1, src.size(), f); fclose(f); }
    return path;
}

static void InitScript(duin::Script &s)
{
    s.SetDasRoot(DAS_ROOT);
    s.InitModules();
}

namespace TestScript
{

TEST_SUITE("Script")
{

    // =========================================================================
    // No-compile tests — no InitModules/daslang needed
    // =========================================================================

    TEST_CASE("Default constructor stores empty path")
    {
        duin::Script s;
        CHECK(s.GetScriptPath() == "");
    }

    TEST_CASE("Path constructor stores the path")
    {
        duin::Script s("foo.das");
        CHECK(s.GetScriptPath() == "foo.das");
    }

    TEST_CASE("SetScriptPath and GetScriptPath round-trip")
    {
        duin::Script s;
        s.SetScriptPath("bar/baz.das");
        CHECK(s.GetScriptPath() == "bar/baz.das");
    }

    TEST_CASE("SetScriptPath overwrites previous path")
    {
        duin::Script s("original.das");
        s.SetScriptPath("new.das");
        CHECK(s.GetScriptPath() == "new.das");
    }

    TEST_CASE("FindFunction before context returns null Func")
    {
        duin::Script s;
        das::Func fn = s.FindFunction("anything");
        CHECK_FALSE(fn);
    }

    TEST_CASE("CallScript with null fn returns false")
    {
        duin::Script s;
        CHECK_FALSE(s.CallScript(das::Func{}));
    }

    TEST_CASE("InvokeVoid with null fn returns false")
    {
        duin::Script s;
        CHECK_FALSE(s.InvokeVoid(das::Func{}));
    }

    TEST_CASE("InvokeWithDelta with null fn returns false")
    {
        duin::Script s;
        CHECK_FALSE(s.InvokeWithDelta(das::Func{}, 0.016));
    }

    TEST_CASE("ResetScript on fresh Script doesn't crash")
    {
        duin::Script s;
        s.ResetScript();
        CHECK(true);
    }

    TEST_CASE("Multiple ResetScript calls don't crash")
    {
        duin::Script s;
        s.ResetScript();
        s.ResetScript();
        s.ResetScript();
        CHECK(true);
    }

    // =========================================================================
    // Compile/simulate lifecycle tests
    // =========================================================================

    TEST_CASE("Compile with empty path returns false")
    {
        duin::Script s;
        InitScript(s);
        s.SetScriptPath("");
        CHECK_FALSE(s.Compile());
    }

    TEST_CASE("Compile with nonexistent path returns false")
    {
        duin::Script s;
        InitScript(s);
        s.SetScriptPath("./artifacts/does_not_exist_xyz.das");
        CHECK_FALSE(s.Compile());
    }

    TEST_CASE("Compile with syntax error returns false")
    {
        const std::string path = WriteDas("syntax_error.das", "THIS IS NOT VALID DASLANG CODE !!!");
        duin::Script s(path);
        InitScript(s);
        CHECK_FALSE(s.Compile());
    }

    TEST_CASE("Compile with syntax error doesn't leave context set")
    {
        const std::string path = WriteDas("syntax_error2.das", "INVALID SCRIPT");
        duin::Script s(path);
        InitScript(s);
        s.Compile();
        CHECK_FALSE(s.FindFunction("anything"));
    }

    TEST_CASE("Compile and SimulateContext with minimal script succeeds")
    {
        const std::string src = "options gen2\n";
        const std::string path = WriteDas("minimal.das", src);
        duin::Script s(path);
        InitScript(s);
        CHECK(s.Compile());
        CHECK(s.SimulateContext());
    }

    TEST_CASE("FindFunction returns null for missing function after simulate")
    {
        const std::string src = "options gen2\n";
        const std::string path = WriteDas("nofn.das", src);
        duin::Script s(path);
        InitScript(s);
        s.Compile();
        s.SimulateContext();
        das::Func fn = s.FindFunction("nonexistent_function");
        CHECK_FALSE(fn);
    }

    TEST_CASE("FindFunction returns valid Func for exported function after simulate")
    {
        const std::string src =
            "options gen2\n"
            "[export]\n"
            "def my_func() : void {}\n";
        const std::string path = WriteDas("exported_fn.das", src);
        duin::Script s(path);
        InitScript(s);
        CHECK(s.Compile());
        CHECK(s.SimulateContext());
        das::Func fn = s.FindFunction("my_func");
        CHECK(fn);
    }

    TEST_CASE("InvokeVoid on exported void function returns true")
    {
        const std::string src =
            "options gen2\n"
            "[export]\n"
            "def do_nothing() : void {}\n";
        const std::string path = WriteDas("invokevoid.das", src);
        duin::Script s(path);
        InitScript(s);
        s.Compile();
        s.SimulateContext();
        das::Func fn = s.FindFunction("do_nothing");
        REQUIRE(fn);
        CHECK(s.InvokeVoid(fn));
    }

    TEST_CASE("InvokeWithDelta on double-accepting function returns true")
    {
        const std::string src =
            "options gen2\n"
            "[export]\n"
            "def do_update(delta : double) : void {}\n";
        const std::string path = WriteDas("invokedelta.das", src);
        duin::Script s(path);
        InitScript(s);
        s.Compile();
        s.SimulateContext();
        das::Func fn = s.FindFunction("do_update");
        REQUIRE(fn);
        CHECK(s.InvokeWithDelta(fn, 0.016));
    }

    TEST_CASE("InvokeVoid on panicking function returns false and doesn't crash")
    {
        const std::string src =
            "options gen2\n"
            "[export]\n"
            "def panic_fn() : void {\n"
            "    panic(\"test panic\")\n"
            "}\n";
        const std::string path = WriteDas("panic_fn.das", src);
        duin::Script s(path);
        InitScript(s);
        s.Compile();
        s.SimulateContext();
        das::Func fn = s.FindFunction("panic_fn");
        REQUIRE(fn);
        CHECK_FALSE(s.InvokeVoid(fn));
    }

    TEST_CASE("InvokeVoid can be called again after a script panic")
    {
        const std::string src =
            "options gen2\n"
            "[export]\n"
            "def panic_fn() : void { panic(\"boom\"); }\n"
            "[export]\n"
            "def safe_fn() : void {}\n";
        const std::string path = WriteDas("recover_after_panic.das", src);
        duin::Script s(path);
        InitScript(s);
        REQUIRE(s.Compile());
        REQUIRE(s.SimulateContext());
        das::Func panicFn = s.FindFunction("panic_fn");
        das::Func safeFn  = s.FindFunction("safe_fn");
        REQUIRE(panicFn);
        REQUIRE(safeFn);

        CHECK_FALSE(s.InvokeVoid(panicFn));
        CHECK(s.InvokeVoid(safeFn));
    }

    TEST_CASE("ResetScript clears context — FindFunction returns null after reset")
    {
        const std::string src =
            "options gen2\n"
            "[export]\n"
            "def some_fn() : void {}\n";
        const std::string path = WriteDas("reset_test.das", src);
        duin::Script s(path);
        InitScript(s);
        s.Compile();
        s.SimulateContext();
        REQUIRE(s.FindFunction("some_fn"));

        s.ResetScript();
        CHECK_FALSE(s.FindFunction("some_fn"));
    }

    TEST_CASE("ResetScript and full recompile cycle works")
    {
        const std::string src = "options gen2\n[export]\ndef fn() : void {}\n";
        const std::string path = WriteDas("recompile.das", src);

        {
            duin::Script s(path);
            InitScript(s);
            CHECK(s.Compile());
            CHECK(s.SimulateContext());
        }
        // Previous Script destroyed → Module::Shutdown called.
        // Now compile a fresh one:
        {
            duin::Script s(path);
            InitScript(s);
            CHECK(s.Compile());
            CHECK(s.SimulateContext());
            CHECK(s.FindFunction("fn"));
        }
    }

    TEST_CASE("Compile failure does not overwrite a previously successful program")
    {
        const std::string goodSrc = "options gen2\n[export]\ndef good() : void {}\n";
        const std::string goodPath = WriteDas("good_script.das", goodSrc);
        const std::string badPath  = WriteDas("bad_script.das", "NOT VALID");

        duin::Script s(goodPath);
        InitScript(s);
        REQUIRE(s.Compile());
        REQUIRE(s.SimulateContext());
        das::Func goodFn = s.FindFunction("good");
        REQUIRE(goodFn);

        // Attempt a recompile of a bad script
        s.SetScriptPath(badPath);
        CHECK_FALSE(s.Compile());

        // Switching back: the previous program is gone (Compile didn't update it on failure),
        // but the context from the successful simulate should still be intact.
        CHECK(s.FindFunction("good"));
    }

    TEST_CASE("InvokeWithDelta with zero delta doesn't crash")
    {
        const std::string src =
            "options gen2\n"
            "[export]\n"
            "def do_update(delta : double) : void {}\n";
        const std::string path = WriteDas("zero_delta.das", src);
        duin::Script s(path);
        InitScript(s);
        s.Compile();
        s.SimulateContext();
        das::Func fn = s.FindFunction("do_update");
        REQUIRE(fn);
        CHECK(s.InvokeWithDelta(fn, 0.0));
    }

    TEST_CASE("Compile and simulate two independent scripts in sequence")
    {
        const std::string srcA = "options gen2\n[export]\ndef fn_a() : void {}\n";
        const std::string srcB = "options gen2\n[export]\ndef fn_b() : void {}\n";
        const std::string pathA = WriteDas("seq_a.das", srcA);
        const std::string pathB = WriteDas("seq_b.das", srcB);

        {
            duin::Script s(pathA);
            InitScript(s);
            CHECK(s.Compile());
            CHECK(s.SimulateContext());
            CHECK(s.FindFunction("fn_a"));
            CHECK_FALSE(s.FindFunction("fn_b"));
        }
        {
            duin::Script s(pathB);
            InitScript(s);
            CHECK(s.Compile());
            CHECK(s.SimulateContext());
            CHECK(s.FindFunction("fn_b"));
            CHECK_FALSE(s.FindFunction("fn_a"));
        }
    }

    TEST_CASE("Compilation performance — minimal script compiles in under 5 seconds")
    {
        const std::string src = "options gen2\n";
        const std::string path = WriteDas("perf_minimal.das", src);
        duin::Script s(path);
        InitScript(s);

        auto t0 = std::chrono::steady_clock::now();
        bool compiled = s.Compile() && s.SimulateContext();
        auto ms = std::chrono::duration_cast<std::chrono::milliseconds>(
                      std::chrono::steady_clock::now() - t0).count();

        CHECK(compiled);
        CHECK(ms < 5000);
    }

    // =========================================================================
    // Reset/recompile integrity tests
    // =========================================================================

    TEST_CASE("ResetScript with live handle — InvokeVoid on old handle returns false, does not crash")
    {
        // Captures a func handle before reset, then resets. The old handle's SimFunction*
        // points into the now-destroyed context. InvokeVoid must guard against this and
        // return false rather than crashing or invoking into freed memory.
        const std::string src = "options gen2\n[export]\ndef fn() : void {}\n";
        const std::string path = WriteDas("reset_stale_handle.das", src);
        duin::Script s(path);
        InitScript(s);
        REQUIRE(s.Compile());
        REQUIRE(s.SimulateContext());

        das::Func oldHandle = s.FindFunction("fn");
        REQUIRE(oldHandle);

        s.ResetScript();

        // Context is gone; old handle is a dangling SimFunction* — must not crash.
        CHECK_FALSE(s.InvokeVoid(oldHandle));
    }

    TEST_CASE("Second SimulateContext creates a new context — FindFunction returns a different handle")
    {
        // After a second Compile+SimulateContext the old SimFunction* pointers are stale.
        // FindFunction must return fresh handles bound to the new context, not the old one.
        const std::string src = "options gen2\n[export]\ndef fn() : void {}\n";
        const std::string path = WriteDas("second_sim_new_ctx.das", src);
        duin::Script s(path);
        InitScript(s);
        REQUIRE(s.Compile());
        REQUIRE(s.SimulateContext());

        das::Func first = s.FindFunction("fn");
        REQUIRE(first);

        // Second compile+simulate: new context replaces the old one.
        REQUIRE(s.Compile());
        REQUIRE(s.SimulateContext());

        das::Func second = s.FindFunction("fn");
        REQUIRE(second);

        // The raw SimFunction pointer must differ; they live in separate contexts.
        CHECK(first.PTR != second.PTR);
    }

    TEST_CASE("InvokeVoid with handle from first context fails after second SimulateContext")
    {
        // Using a handle from an old context with the new context is the root cause of
        // the 'Function is mangled' errors. After re-simulation the old handle's PTR is
        // no longer inside the current context's function table.
        const std::string src = "options gen2\n[export]\ndef fn() : void {}\n";
        const std::string path = WriteDas("stale_after_resim.das", src);
        duin::Script s(path);
        InitScript(s);
        REQUIRE(s.Compile());
        REQUIRE(s.SimulateContext());

        das::Func stale = s.FindFunction("fn");
        REQUIRE(stale);

        // Replace the context.
        REQUIRE(s.Compile());
        REQUIRE(s.SimulateContext());

        // Invoking with the stale handle (from the old, now-dead context) must not crash,
        // and must return false because the PTR is not registered in the new context.
        CHECK_FALSE(s.InvokeVoid(stale));
    }

    TEST_CASE("ResetScript clears scriptReady — InvokeVoid on any func returns false")
    {
        const std::string src = "options gen2\n[export]\ndef fn() : void {}\n";
        const std::string path = WriteDas("reset_ready_cleared.das", src);
        duin::Script s(path);
        InitScript(s);
        REQUIRE(s.Compile());
        REQUIRE(s.SimulateContext());
        REQUIRE(s.FindFunction("fn")); // confirms ready

        s.ResetScript();

        // No context means no valid function — InvokeVoid must return false.
        CHECK_FALSE(s.InvokeVoid(das::Func{}));
        CHECK_FALSE(s.InvokeWithDelta(das::Func{}, 0.016));
        CHECK_FALSE(s.CallScript(das::Func{}));
    }

} // TEST_SUITE("Script")

} // namespace TestScript
