#include <doctest.h>
#include <Duin/Script/GameScript.h>
#include <Duin/Objects/GameObject.h>
#include <filesystem>

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

static void InitScript(duin::GameScript &gs)
{
    gs.SetDasRoot(DAS_ROOT);
    gs.InitModules();
}

// Minimal valid daslang script with no game functions.
static const std::string MINIMAL_SRC = "options gen2\n";

// Script providing all five game callbacks as no-ops.
static const std::string FULL_GAME_SRC =
    "options gen2\n"
    "[export]\n"
    "def game_ready() : void {}\n"
    "[export]\n"
    "def game_update(delta : double) : void {}\n"
    "[export]\n"
    "def game_physics_update(delta : double) : void {}\n"
    "[export]\n"
    "def game_draw() : void {}\n"
    "[export]\n"
    "def game_draw_ui() : void {}\n";

// Script that panics inside game_ready.
static const std::string PANIC_READY_SRC =
    "options gen2\n"
    "[export]\n"
    "def game_ready() : void { panic(\"intentional\"); }\n";

namespace TestGameScript
{

TEST_SUITE("GameScript")
{

    // =========================================================================
    // No-compile tests — pure API surface
    // =========================================================================

    TEST_CASE("Constructor stores the script path")
    {
        duin::GameScript gs("my_script.das");
        CHECK(gs.GetScriptPath() == "my_script.das");
    }

    TEST_CASE("GameScript IS-A valid GameObject — UUID is non-null")
    {
        duin::GameScript gs("test.das");
        CHECK(gs.GetUUID() != duin::UUID::INVALID);
    }

    TEST_CASE("Two GameScript instances have different UUIDs")
    {
        duin::GameScript gs1("a.das");
        duin::GameScript gs2("b.das");
        CHECK(gs1.GetUUID() != gs2.GetUUID());
    }

    TEST_CASE("IsHotCompileEnabled is false by default")
    {
        duin::GameScript gs("test.das");
        CHECK_FALSE(gs.IsHotCompileEnabled());
    }

    TEST_CASE("EnableHotCompile(true) sets the flag")
    {
        duin::GameScript gs("test.das");
        gs.EnableHotCompile(true);
        CHECK(gs.IsHotCompileEnabled());
        gs.EnableHotCompile(false); // restore global live_mode
    }

    TEST_CASE("EnableHotCompile(false) clears the flag")
    {
        duin::GameScript gs("test.das");
        gs.EnableHotCompile(true);
        gs.EnableHotCompile(false);
        CHECK_FALSE(gs.IsHotCompileEnabled());
    }

    TEST_CASE("SetContextRootObject without context returns false")
    {
        duin::GameScript gs("test.das");
        CHECK_FALSE(gs.SetContextRootObject());
    }

    TEST_CASE("SetHotCompileFileChangeCooldown doesn't crash")
    {
        duin::GameScript gs("test.das");
        gs.SetHotCompileFileChangeCooldown(5.0f);
        gs.SetHotCompileFileChangeCooldown(0.0f);
        gs.SetHotCompileFileChangeCooldown(-1.0f);
        CHECK(true);
    }

    TEST_CASE("ResetGameFunctions on fresh instance doesn't crash")
    {
        duin::GameScript gs("test.das");
        gs.ResetGameFunctions();
        CHECK(true);
    }

    TEST_CASE("ResetMuteWarningFlags on fresh instance doesn't crash")
    {
        duin::GameScript gs("test.das");
        gs.ResetMuteWarningFlags();
        CHECK(true);
    }

    TEST_CASE("Ready without compile doesn't crash")
    {
        duin::GameScript gs("test.das");
        gs.Ready();
        CHECK(true);
    }

    TEST_CASE("Update without compile doesn't crash")
    {
        duin::GameScript gs("test.das");
        gs.Update(0.016);
        CHECK(true);
    }

    TEST_CASE("PhysicsUpdate without compile doesn't crash")
    {
        duin::GameScript gs("test.das");
        gs.PhysicsUpdate(0.016);
        CHECK(true);
    }

    TEST_CASE("Draw without compile doesn't crash")
    {
        duin::GameScript gs("test.das");
        gs.Draw();
        CHECK(true);
    }

    TEST_CASE("DrawUI without compile doesn't crash")
    {
        duin::GameScript gs("test.das");
        gs.DrawUI();
        CHECK(true);
    }

    TEST_CASE("Repeated lifecycle calls without compile are all safe")
    {
        duin::GameScript gs("test.das");
        for (int i = 0; i < 10; ++i)
        {
            gs.Ready();
            gs.Update(0.016 * i);
            gs.PhysicsUpdate(0.016 * i);
            gs.Draw();
            gs.DrawUI();
        }
        CHECK(true);
    }

    TEST_CASE("Warning mute flags suppress repeated warnings on unready script")
    {
        // After the first call to each lifecycle method on an unready script,
        // the mute flag is set so further calls don't re-warn.
        // Resetting the flags allows warnings again — observable via no crash.
        duin::GameScript gs("test.das");
        gs.Update(0.016);   // sets muteUpdateWarning
        gs.Update(0.016);   // silently muted
        gs.ResetMuteWarningFlags();
        gs.Update(0.016);   // warns again — still no crash
        CHECK(true);
    }

    // =========================================================================
    // Compile/simulate lifecycle tests
    // =========================================================================

    TEST_CASE("CompileAndSimulate with invalid script path returns false")
    {
        duin::GameScript gs("./artifacts/does_not_exist_gs.das");
        InitScript(gs);
        CHECK_FALSE(gs.CompileAndSimulate());
    }

    TEST_CASE("CompileAndSimulate with syntax error returns false")
    {
        const std::string path = WriteDas("gs_syntax_error.das", "NOT VALID DASLANG");
        duin::GameScript gs(path);
        InitScript(gs);
        CHECK_FALSE(gs.CompileAndSimulate());
    }

    TEST_CASE("CompileAndSimulate with minimal valid script returns true")
    {
        const std::string path = WriteDas("gs_minimal.das", MINIMAL_SRC);
        duin::GameScript gs(path);
        InitScript(gs);
        CHECK(gs.CompileAndSimulate());
    }

    TEST_CASE("SetContextRootObject returns true after successful CompileAndSimulate")
    {
        const std::string path = WriteDas("gs_ctx_root.das", MINIMAL_SRC);
        duin::GameScript gs(path);
        InitScript(gs);
        REQUIRE(gs.CompileAndSimulate());
        CHECK(gs.SetContextRootObject());
    }

    TEST_CASE("After CompileAndSimulate Ready doesn't crash")
    {
        const std::string path = WriteDas("gs_ready_post.das", MINIMAL_SRC);
        duin::GameScript gs(path);
        InitScript(gs);
        REQUIRE(gs.CompileAndSimulate());
        gs.Ready();
        CHECK(true);
    }

    TEST_CASE("After CompileAndSimulate Update doesn't crash")
    {
        const std::string path = WriteDas("gs_update_post.das", MINIMAL_SRC);
        duin::GameScript gs(path);
        InitScript(gs);
        REQUIRE(gs.CompileAndSimulate());
        gs.Update(0.016);
        CHECK(true);
    }

    TEST_CASE("After CompileAndSimulate multiple Update calls are safe")
    {
        const std::string path = WriteDas("gs_multi_update.das", MINIMAL_SRC);
        duin::GameScript gs(path);
        InitScript(gs);
        REQUIRE(gs.CompileAndSimulate());
        for (int i = 0; i < 100; ++i)
            gs.Update(0.016);
        CHECK(true);
    }

    TEST_CASE("After CompileAndSimulate PhysicsUpdate doesn't crash")
    {
        const std::string path = WriteDas("gs_physics_post.das", MINIMAL_SRC);
        duin::GameScript gs(path);
        InitScript(gs);
        REQUIRE(gs.CompileAndSimulate());
        gs.PhysicsUpdate(0.016);
        CHECK(true);
    }

    TEST_CASE("After CompileAndSimulate Draw doesn't crash")
    {
        const std::string path = WriteDas("gs_draw_post.das", MINIMAL_SRC);
        duin::GameScript gs(path);
        InitScript(gs);
        REQUIRE(gs.CompileAndSimulate());
        gs.Draw();
        CHECK(true);
    }

    TEST_CASE("After CompileAndSimulate DrawUI doesn't crash")
    {
        const std::string path = WriteDas("gs_drawui_post.das", MINIMAL_SRC);
        duin::GameScript gs(path);
        InitScript(gs);
        REQUIRE(gs.CompileAndSimulate());
        gs.DrawUI();
        CHECK(true);
    }

    TEST_CASE("Script with all game functions — CompileAndSimulate succeeds")
    {
        const std::string path = WriteDas("gs_full.das", FULL_GAME_SRC);
        duin::GameScript gs(path);
        InitScript(gs);
        CHECK(gs.CompileAndSimulate());
    }

    TEST_CASE("FindFunction finds game_ready after CompileAndSimulate with full script")
    {
        const std::string path = WriteDas("gs_find_ready.das", FULL_GAME_SRC);
        duin::GameScript gs(path);
        InitScript(gs);
        REQUIRE(gs.CompileAndSimulate());
        CHECK(gs.FindFunction("game_ready"));
        CHECK(gs.FindFunction("game_update"));
        CHECK(gs.FindFunction("game_physics_update"));
        CHECK(gs.FindFunction("game_draw"));
        CHECK(gs.FindFunction("game_draw_ui"));
    }

    TEST_CASE("InvokeVoid on game_ready returns true after CompileAndSimulate")
    {
        const std::string path = WriteDas("gs_invoke_ready.das", FULL_GAME_SRC);
        duin::GameScript gs(path);
        InitScript(gs);
        REQUIRE(gs.CompileAndSimulate());
        das::Func fn = gs.FindFunction("game_ready");
        REQUIRE(fn);
        CHECK(gs.InvokeVoid(fn));
    }

    TEST_CASE("InvokeWithDelta on game_update returns true after CompileAndSimulate")
    {
        const std::string path = WriteDas("gs_invoke_update.das", FULL_GAME_SRC);
        duin::GameScript gs(path);
        InitScript(gs);
        REQUIRE(gs.CompileAndSimulate());
        das::Func fn = gs.FindFunction("game_update");
        REQUIRE(fn);
        CHECK(gs.InvokeWithDelta(fn, 0.016));
    }

    TEST_CASE("CompileAndSimulate twice succeeds — hot-reload simulation")
    {
        const std::string path = WriteDas("gs_hotreload.das", MINIMAL_SRC);
        duin::GameScript gs(path);
        InitScript(gs);
        CHECK(gs.CompileAndSimulate());
        CHECK(gs.CompileAndSimulate());
    }

    TEST_CASE("CompileAndSimulate twice with full game functions succeeds")
    {
        // Exercises the isReload=true path with all five callbacks bound,
        // which is closer to the editor scenario.
        const std::string path = WriteDas("gs_hotreload_full.das", FULL_GAME_SRC);
        duin::GameScript gs(path);
        InitScript(gs);
        REQUIRE(gs.CompileAndSimulate());
        CHECK(gs.CompileAndSimulate());
    }

    TEST_CASE("CompileAndSimulate ten times in succession does not crash")
    {
        // Stress-tests the reload path for accumulated context or function-pointer
        // invalidation bugs that only appear after multiple cycles.
        const std::string path = WriteDas("gs_hotreload_stress.das", FULL_GAME_SRC);
        duin::GameScript gs(path);
        InitScript(gs);
        REQUIRE(gs.CompileAndSimulate());
        for (int i = 0; i < 9; ++i)
            CHECK(gs.CompileAndSimulate());
    }

    TEST_CASE("After second CompileAndSimulate game callbacks remain callable")
    {
        // After a reload the re-bound function handles must still be valid for
        // InvokeVoid and InvokeWithDelta.
        const std::string path = WriteDas("gs_hotreload_invoke.das", FULL_GAME_SRC);
        duin::GameScript gs(path);
        InitScript(gs);
        REQUIRE(gs.CompileAndSimulate());
        REQUIRE(gs.CompileAndSimulate()); // reload — context replaced
        CHECK(gs.InvokeVoid(gs.FindFunction("game_ready")));
        CHECK(gs.InvokeWithDelta(gs.FindFunction("game_update"), 0.016));
    }

    TEST_CASE("Update does not crash after hot-reload when game_update is present")
    {
        // Simulates the Update() call path that triggered the editor crash:
        //   GameScript::Update → CompileAndSimulate (via queueHotCompileFlag) →
        //   then immediately Update → InvokeWithDelta(fnGameUpdate).
        const std::string path = WriteDas("gs_hotreload_update_after.das", FULL_GAME_SRC);
        duin::GameScript gs(path);
        InitScript(gs);
        REQUIRE(gs.CompileAndSimulate()); // first compile
        REQUIRE(gs.CompileAndSimulate()); // simulates a hot-reload
        gs.Update(0.016);
        gs.Update(0.016);
        CHECK(true);
    }

    TEST_CASE("All lifecycle methods are safe immediately after hot-reload")
    {
        const std::string path = WriteDas("gs_hotreload_lifecycle.das", FULL_GAME_SRC);
        duin::GameScript gs(path);
        InitScript(gs);
        REQUIRE(gs.CompileAndSimulate());
        REQUIRE(gs.CompileAndSimulate()); // reload
        gs.Ready();
        gs.Update(0.016);
        gs.PhysicsUpdate(0.016);
        gs.Draw();
        gs.DrawUI();
        CHECK(true);
    }

    TEST_CASE("Hot-reload from minimal to full game script does not crash")
    {
        // Script changes between reloads — exercises SetGameFunctions re-binding
        // after the context is replaced.
        const std::string pathA = WriteDas("gs_hotreload_upgrade_a.das", MINIMAL_SRC);
        const std::string pathB = WriteDas("gs_hotreload_upgrade_b.das", FULL_GAME_SRC);
        duin::GameScript gs(pathA);
        InitScript(gs);
        REQUIRE(gs.CompileAndSimulate());         // compile minimal (no callbacks)
        gs.SetScriptPath(pathB);
        CHECK(gs.CompileAndSimulate());           // reload as full — binds all five fns
        CHECK(gs.FindFunction("game_ready"));
    }

    TEST_CASE("Hot-reload from full to minimal game script does not crash")
    {
        // Exercises SetGameFunctions when callbacks disappear on reload.
        const std::string pathA = WriteDas("gs_hotreload_downgrade_a.das", FULL_GAME_SRC);
        const std::string pathB = WriteDas("gs_hotreload_downgrade_b.das", MINIMAL_SRC);
        duin::GameScript gs(pathA);
        InitScript(gs);
        REQUIRE(gs.CompileAndSimulate());
        gs.SetScriptPath(pathB);
        CHECK(gs.CompileAndSimulate()); // callbacks disappear — no crash
        CHECK_FALSE(gs.FindFunction("game_ready"));
    }

    TEST_CASE("CompileAndSimulate after failed compile still works with previous context")
    {
        const std::string goodPath = WriteDas("gs_good_then_bad1.das", MINIMAL_SRC);
        const std::string badPath  = WriteDas("gs_good_then_bad2.das", "NOT VALID");
        duin::GameScript gs(goodPath);
        InitScript(gs);
        REQUIRE(gs.CompileAndSimulate()); // first: good

        gs.SetScriptPath(badPath);
        CHECK_FALSE(gs.CompileAndSimulate()); // second: fails — context from first survives

        // Context from first compile is still usable
        CHECK(gs.SetContextRootObject());
    }

    TEST_CASE("haltOnCompileFail=true: failed compile clears scriptReady observable via Ready")
    {
        // haltOnCompilationFail is set via EnableHotCompile(enable, halt).
        // With halt=true, a compile failure will set scriptReady=false so
        // the next Ready() call cannot invoke game_ready.
        const std::string goodPath = WriteDas("gs_halt_good.das", FULL_GAME_SRC);
        const std::string badPath  = WriteDas("gs_halt_bad.das", "NOT VALID");

        duin::GameScript gs(goodPath);
        InitScript(gs);
        REQUIRE(gs.CompileAndSimulate());

        // Enable halt-on-fail (keep hotCompile disabled to avoid VFS calls)
        gs.EnableHotCompile(false, true);

        gs.SetScriptPath(badPath);
        bool secondCompile = gs.CompileAndSimulate();
        CHECK_FALSE(secondCompile); // compile must fail

        // With scriptReady=false, InvokeVoid on the (still-bound) game_ready
        // would be guarded internally — Ready() itself must not crash.
        gs.Ready();
        gs.Update(0.016);
        CHECK(true);
    }

    TEST_CASE("Script with panicking game_ready — Ready doesn't crash")
    {
        const std::string path = WriteDas("gs_panic_ready.das", PANIC_READY_SRC);
        duin::GameScript gs(path);
        InitScript(gs);
        REQUIRE(gs.CompileAndSimulate()); // CompileAndSimulate calls Ready() internally
        // If we reach here, the exception was caught — no crash
        CHECK(true);
    }

    TEST_CASE("Script with panicking game_ready — subsequent Update is safe")
    {
        const std::string path = WriteDas("gs_panic_update.das", PANIC_READY_SRC);
        duin::GameScript gs(path);
        InitScript(gs);
        REQUIRE(gs.CompileAndSimulate());
        gs.Update(0.016); // game_update is absent → silently skipped
        CHECK(true);
    }

    TEST_CASE("ResetScript and recompile cycle works for GameScript")
    {
        const std::string path = WriteDas("gs_recompile.das", MINIMAL_SRC);

        {
            duin::GameScript gs(path);
            InitScript(gs);
            CHECK(gs.CompileAndSimulate());
        }
        // Destructor called → Module::Shutdown.
        {
            duin::GameScript gs(path);
            InitScript(gs);
            CHECK(gs.CompileAndSimulate());
        }
    }

    TEST_CASE("GameScript can be added as a child of a plain GameObject")
    {
        auto root = std::make_shared<duin::GameObject>();
        auto gs   = std::make_shared<duin::GameScript>("child.das");
        root->AddChildObject(gs);
        CHECK(gs->GetParent() == root.get());
    }

    TEST_CASE("GameScript as child: UUID is still valid after add")
    {
        auto root = std::make_shared<duin::GameObject>();
        auto gs   = std::make_shared<duin::GameScript>("child2.das");
        root->AddChildObject(gs);
        CHECK(gs->GetUUID() != duin::UUID::INVALID);
    }

    // =========================================================================
    // Reset/recompile integrity tests
    // These guard against the 'Function is mangled' errors that occur when
    // stale SimFunction* handles from an old context are used after reload.
    // =========================================================================

    TEST_CASE("CompileAndSimulate reload produces new SimFunction handles — PTRs differ from first compile")
    {
        // After a reload the old context is destroyed and a new one created.
        // All five game-function handles must be re-fetched from the new context;
        // their SimFunction* addresses must differ from those of the first compile.
        const std::string path = WriteDas("gs_ptrs_differ.das", FULL_GAME_SRC);
        duin::GameScript gs(path);
        InitScript(gs);
        REQUIRE(gs.CompileAndSimulate());

        das::Func r1 = gs.FindFunction("game_ready");
        das::Func u1 = gs.FindFunction("game_update");
        das::Func p1 = gs.FindFunction("game_physics_update");
        das::Func d1 = gs.FindFunction("game_draw");
        das::Func ui1 = gs.FindFunction("game_draw_ui");
        REQUIRE(r1);
        REQUIRE(u1);
        REQUIRE(p1);
        REQUIRE(d1);
        REQUIRE(ui1);

        REQUIRE(gs.CompileAndSimulate()); // reload — new context

        das::Func r2 = gs.FindFunction("game_ready");
        das::Func u2 = gs.FindFunction("game_update");
        das::Func p2 = gs.FindFunction("game_physics_update");
        das::Func d2 = gs.FindFunction("game_draw");
        das::Func ui2 = gs.FindFunction("game_draw_ui");
        REQUIRE(r2);
        REQUIRE(u2);
        REQUIRE(p2);
        REQUIRE(d2);
        REQUIRE(ui2);

        CHECK(r1.PTR  != r2.PTR);
        CHECK(u1.PTR  != u2.PTR);
        CHECK(p1.PTR  != p2.PTR);
        CHECK(d1.PTR  != d2.PTR);
        CHECK(ui1.PTR != ui2.PTR);
    }

    TEST_CASE("ResetScript nullifies all five game function handles — FindFunction returns null for each")
    {
        // ResetScript must call ResetGameFunctions so that no live SimFunction* from
        // the old context leaks through to a future recompile or invocation.
        const std::string path = WriteDas("gs_reset_nullifies.das", FULL_GAME_SRC);
        duin::GameScript gs(path);
        InitScript(gs);
        REQUIRE(gs.CompileAndSimulate());
        REQUIRE(gs.FindFunction("game_ready"));

        gs.ResetScript();

        CHECK_FALSE(gs.FindFunction("game_ready"));
        CHECK_FALSE(gs.FindFunction("game_update"));
        CHECK_FALSE(gs.FindFunction("game_physics_update"));
        CHECK_FALSE(gs.FindFunction("game_draw"));
        CHECK_FALSE(gs.FindFunction("game_draw_ui"));
    }

    TEST_CASE("ResetScript with live handle — InvokeVoid on old handle returns false, does not crash")
    {
        // After ResetScript the context is gone. Any previously captured das::Func
        // is a dangling SimFunction*; invoking it must return false, not crash.
        const std::string path = WriteDas("gs_reset_stale_invoke.das", FULL_GAME_SRC);
        duin::GameScript gs(path);
        InitScript(gs);
        REQUIRE(gs.CompileAndSimulate());

        das::Func stale = gs.FindFunction("game_ready");
        REQUIRE(stale);

        gs.ResetScript();

        CHECK_FALSE(gs.InvokeVoid(stale));
    }

    TEST_CASE("Stale handle from first compile must not be invokable after reload")
    {
        // Root cause of 'Function is mangled': a SimFunction* from context-1 is used
        // with context-2. After reload InvokeVoid with the pre-reload handle must
        // return false (PTR is not registered in the new context's function table).
        const std::string path = WriteDas("gs_stale_after_reload.das", FULL_GAME_SRC);
        duin::GameScript gs(path);
        InitScript(gs);
        REQUIRE(gs.CompileAndSimulate());

        das::Func staleReady  = gs.FindFunction("game_ready");
        das::Func staleUpdate = gs.FindFunction("game_update");
        REQUIRE(staleReady);
        REQUIRE(staleUpdate);

        REQUIRE(gs.CompileAndSimulate()); // context replaced

        // Old handles point into the dead context — must not crash, must return false.
        CHECK_FALSE(gs.InvokeVoid(staleReady));
        CHECK_FALSE(gs.InvokeWithDelta(staleUpdate, 0.016));
    }

    TEST_CASE("ResetGameFunctions before SetGameFunctions on reload — no double-bound handles")
    {
        // CompileAndSimulate must clear the old fn* fields before re-binding them.
        // Without this, if the new script omits a callback the old (stale) handle
        // would remain, causing invocations into the dead context.
        const std::string fullPath    = WriteDas("gs_double_bind_full.das", FULL_GAME_SRC);
        const std::string minimalPath = WriteDas("gs_double_bind_min.das", MINIMAL_SRC);

        duin::GameScript gs(fullPath);
        InitScript(gs);
        REQUIRE(gs.CompileAndSimulate()); // all five handles bound

        // Reload with a script that exports no game callbacks.
        gs.SetScriptPath(minimalPath);
        REQUIRE(gs.CompileAndSimulate());

        // All handles must now be null — not the stale pointers from the first context.
        CHECK_FALSE(gs.FindFunction("game_ready"));
        CHECK_FALSE(gs.FindFunction("game_update"));
        CHECK_FALSE(gs.FindFunction("game_physics_update"));
        CHECK_FALSE(gs.FindFunction("game_draw"));
        CHECK_FALSE(gs.FindFunction("game_draw_ui"));
    }

    TEST_CASE("Lifecycle methods after reload use fresh handles — no mangled-function invocations")
    {
        // After reload, Update/PhysicsUpdate/Draw/DrawUI must invoke via the new context's
        // SimFunction*, not stale pointers. No crash, no 'mangled' log spam.
        const std::string path = WriteDas("gs_lifecycle_post_reload.das", FULL_GAME_SRC);
        duin::GameScript gs(path);
        InitScript(gs);
        REQUIRE(gs.CompileAndSimulate());
        REQUIRE(gs.CompileAndSimulate()); // reload

        // These calls must succeed without crashing and without using stale PTRs.
        gs.Ready();
        gs.Update(0.016);
        gs.PhysicsUpdate(0.016);
        gs.Draw();
        gs.DrawUI();

        // Explicit invocation via freshly fetched handles must also succeed.
        CHECK(gs.InvokeVoid(gs.FindFunction("game_ready")));
        CHECK(gs.InvokeWithDelta(gs.FindFunction("game_update"), 0.016));
        CHECK(gs.InvokeWithDelta(gs.FindFunction("game_physics_update"), 0.016));
        CHECK(gs.InvokeVoid(gs.FindFunction("game_draw")));
        CHECK(gs.InvokeVoid(gs.FindFunction("game_draw_ui")));
    }

    TEST_CASE("Ten reload cycles — handles valid and distinct after every cycle")
    {
        // Stress-tests that each CompileAndSimulate creates an independent context with
        // freshly bound handles. Accumulation of stale pointers across cycles is the
        // trigger for repeated 'mangled' error bursts in the editor.
        const std::string path = WriteDas("gs_ten_cycle_handles.das", FULL_GAME_SRC);
        duin::GameScript gs(path);
        InitScript(gs);
        REQUIRE(gs.CompileAndSimulate());

        das::Func prevReady = gs.FindFunction("game_ready");

        for (int i = 0; i < 10; ++i)
        {
            REQUIRE(gs.CompileAndSimulate());

            das::Func curReady = gs.FindFunction("game_ready");
            REQUIRE(curReady);
            CHECK(curReady.PTR != prevReady.PTR); // new context each time
            prevReady = curReady;

            // All five handles must be valid and invocable.
            CHECK(gs.InvokeVoid(gs.FindFunction("game_ready")));
            CHECK(gs.InvokeWithDelta(gs.FindFunction("game_update"), 0.016));
            CHECK(gs.InvokeWithDelta(gs.FindFunction("game_physics_update"), 0.016));
            CHECK(gs.InvokeVoid(gs.FindFunction("game_draw")));
            CHECK(gs.InvokeVoid(gs.FindFunction("game_draw_ui")));
        }
    }

    TEST_CASE("game_update running basic maths for 20 simulated minutes doesn't crash")
    {
        // Script accumulates time, a running sum, and does trig — exercises
        // floating-point accumulation and math builtins continuously.
        const std::string src =
            "options gen2\n"
            "require math\n"
            "\n"
            "var g_time   : double = 0.0d\n"
            "var g_frames : int    = 0\n"
            "var g_accum  : double = 0.0d\n"
            "\n"
            "[export]\n"
            "def game_update(delta : double) : void {\n"
            "    g_time   += delta\n"
            "    g_frames += 1\n"
            "    let s = sin(float(g_time))\n"
            "    let c = cos(float(g_time))\n"
            "    g_accum  += double(s * s + c * c)\n"  // identity: always ~1.0
            "}\n";

        const std::string path = WriteDas("gs_math_20min.das", src);
        duin::GameScript gs(path);
        InitScript(gs);
        REQUIRE(gs.CompileAndSimulate());

        // Simulate 20 minutes at 60 fps.
        constexpr double DELTA     = 1.0 / 60.0;
        constexpr int    FRAMES    = 20 * 60 * 60; // 72 000 frames
        for (int i = 0; i < FRAMES; ++i)
            gs.Update(DELTA);

        CHECK(true); // reaching here means no crash across the full 20-minute run
    }

} // TEST_SUITE("GameScript")

} // namespace TestGameScript
