#include <doctest.h>
#include <Duin/Script/GameScript.h>
#include <Duin/Script/DNLog/Module_DnLog.h>
#include <Duin/Script/GameObject/Module_DnGameObject.h>
#include <Duin/Core/Application.h>

static const std::string ARTIFACTS_DIR = "./artifacts";

// Helper: write a .das file under ARTIFACTS_DIR and return its path.
static std::string WriteDas(const std::string &name, const std::string &src)
{
    std::string path = ARTIFACTS_DIR + "/" + name;
    FILE *f = fopen(path.c_str(), "w");
    if (f) { fwrite(src.data(), 1, src.size(), f); fclose(f); }
    return path;
}

namespace TestGameScript
{

TEST_SUITE("GameScript")
{
    TEST_CASE("CompileAndSimulate succeeds with a valid script providing all game functions")
    {
        auto path = WriteDas("_test_gamescript_full.das",
            "options gen2\n"
            "[export] def game_ready() {}\n"
            "[export] def game_update() {}\n"
            "[export] def game_physics_update() {}\n"
            "[export] def game_draw() {}\n"
            "[export] def game_draw_ui() {}\n");

        duin::GameScript gs(path);
        gs.InitModules([]() {
            NEED_MODULE(Module_DnLog); NEED_MODULE(Module_DnGameObject);
        });
        gs.CompileAndSimulate();

        CHECK(gs.FindFunction("game_ready") != nullptr);
        CHECK(gs.FindFunction("game_update") != nullptr);
        CHECK(gs.FindFunction("game_physics_update") != nullptr);
        CHECK(gs.FindFunction("game_draw") != nullptr);
        CHECK(gs.FindFunction("game_draw_ui") != nullptr);

        gs.ResetScript();
    }

    TEST_CASE("CompileAndSimulate handles a script with no game functions gracefully")
    {
        auto path = WriteDas("_test_gamescript_empty.das",
            "options gen2\n"
            "[export] def main() {}\n");

        duin::GameScript gs(path);
        gs.InitModules([]() {
            NEED_MODULE(Module_DnLog); NEED_MODULE(Module_DnGameObject);
        });
        gs.CompileAndSimulate();

        CHECK(gs.FindFunction("game_ready") == nullptr);
        CHECK(gs.FindFunction("game_update") == nullptr);

        gs.ResetScript();
    }

    TEST_CASE("CompileAndSimulate does not crash on a broken script")
    {
        auto path = WriteDas("_test_gamescript_broken.das",
            "options gen2\n"
            "def broken( { syntax error here\n");

        duin::GameScript gs(path);
        gs.InitModules([]() {
            NEED_MODULE(Module_DnLog); NEED_MODULE(Module_DnGameObject);
        });
        CHECK_NOTHROW(gs.CompileAndSimulate());
        gs.ResetScript();
    }

    TEST_CASE("EnableHotCompile and IsHotCompileEnabled round-trip correctly")
    {
        auto path = WriteDas("_test_gamescript_hot.das",
            "options gen2\n"
            "[export] def game_ready() {}\n");

        duin::GameScript gs(path);
        gs.InitModules([]() {
            NEED_MODULE(Module_DnLog); NEED_MODULE(Module_DnGameObject);
        });

        CHECK_FALSE(gs.IsHotCompileEnabled());
        gs.EnableHotCompile(true);
        CHECK(gs.IsHotCompileEnabled());
        gs.EnableHotCompile(false);
        CHECK_FALSE(gs.IsHotCompileEnabled());

        gs.ResetScript();
    }

    TEST_CASE("Ready, Update, PhysicsUpdate, Draw, DrawUI do not crash when script is not ready")
    {
        duin::GameScript gs(ARTIFACTS_DIR + "/_no_such_script.das");
        gs.InitModules([]() {
            NEED_MODULE(Module_DnLog); NEED_MODULE(Module_DnGameObject);
        });

        CHECK_NOTHROW(gs.Ready());
        CHECK_NOTHROW(gs.Update(0.016));
        CHECK_NOTHROW(gs.PhysicsUpdate(0.016));
        CHECK_NOTHROW(gs.Draw());
        CHECK_NOTHROW(gs.DrawUI());

        gs.ResetScript();
    }

    TEST_CASE("ResetScript can be called multiple times without crashing")
    {
        auto path = WriteDas("_test_gamescript_exit.das",
            "options gen2\n"
            "[export] def game_ready() {}\n");

        duin::GameScript gs(path);
        gs.InitModules([]() {
            NEED_MODULE(Module_DnLog); NEED_MODULE(Module_DnGameObject);
        });
        gs.CompileAndSimulate();

        CHECK_NOTHROW(gs.ResetScript());
        CHECK_NOTHROW(gs.ResetScript());
    }
}

// ============================================================================
// Double-ready bug regression tests
//
// The bug: CompileAndSimulate(skipReady=false) calls game_ready() directly,
// then EnginePostReady() dispatches through the tree and calls game_ready()
// a second time, resulting in script objects being created twice.
//
// Desired behaviour: game_ready() is called exactly once per compile cycle.
// ============================================================================

TEST_SUITE("GameScript Ready Dispatch")
{
    // Helper: Application subclass that exposes EnginePostReady for testing.
    class TestApp : public duin::Application {};

    TEST_CASE("CompileAndSimulate(skipReady=false) + EnginePostReady calls game_ready twice [known bug]")
    {
        // This test documents the double-ready bug.
        // A script that creates one child object in game_ready should create
        // exactly 1 child. With the bug, game_ready fires twice (once from
        // CompileAndSimulate, once from the tree dispatch in EnginePostReady),
        // producing 2 children instead. When the bug is fixed, update this
        // test to assert == 1.
        auto childPath = WriteDas("_test_gs_double_ready_child.das",
            "options gen2\n"
            "options rtti\n"
            "require dn_gameobject\n"
            "require rtti\n"
            "class Counter : dn_GameObject {\n"
            "    def override _init() {}\n"
            "    def override _ready() {}\n"
            "    def override _update(delta:double) {}\n"
            "    def override _physics_update(delta:double) {}\n"
            "    def override _draw() {}\n"
            "    def override _draw_ui() {}\n"
            "    def override _debug() {}\n"
            "}\n"
            "[export] def game_ready() { dn_gameobject_add_root_child(new Counter()); }\n"
            "[export] def game_update() {}\n"
            "[export] def game_physics_update() {}\n"
            "[export] def game_draw() {}\n"
            "[export] def game_draw_ui() {}\n");

        TestApp app;
        auto gs = app.CreateChildObject<duin::GameScript>(childPath);
        gs->InitModules([]() {
            NEED_MODULE(Module_DnLog); NEED_MODULE(Module_DnGameObject);
        });

        // skipReady=false: CompileAndSimulate calls Ready() itself
        gs->CompileAndSimulate(false);
        // EnginePostReady dispatches tree → calls Ready() on gs again
        app.EnginePostReady();

        // Desired behaviour: exactly 1 child created (game_ready called once)
        // Current (buggy) behaviour: 2 children created (game_ready called twice)
        CHECK(gs->GetChildrenCount() == 1);

        gs->ResetScript();
    }

    TEST_CASE("CompileAndSimulate(skipReady=true) + EnginePostReady calls game_ready exactly once")
    {
        // Correct usage: skipReady=true defers Ready() to the tree dispatch.
        auto childPath = WriteDas("_test_gs_skip_ready_child.das",
            "options gen2\n"
            "options rtti\n"
            "require dn_gameobject\n"
            "require rtti\n"
            "class Counter : dn_GameObject {\n"
            "    def override _init() {}\n"
            "    def override _ready() {}\n"
            "    def override _update(delta:double) {}\n"
            "    def override _physics_update(delta:double) {}\n"
            "    def override _draw() {}\n"
            "    def override _draw_ui() {}\n"
            "    def override _debug() {}\n"
            "}\n"
            "[export] def game_ready() { dn_gameobject_add_root_child(new Counter()); }\n"
            "[export] def game_update() {}\n"
            "[export] def game_physics_update() {}\n"
            "[export] def game_draw() {}\n"
            "[export] def game_draw_ui() {}\n");

        TestApp app;
        auto gs = app.CreateChildObject<duin::GameScript>(childPath);
        gs->InitModules([]() {
            NEED_MODULE(Module_DnLog); NEED_MODULE(Module_DnGameObject);
        });

        gs->CompileAndSimulate(true);   // skip Ready() here
        app.EnginePostReady();          // tree dispatch fires Ready() once

        CHECK(gs->GetChildrenCount() == 1);

        gs->ResetScript();
    }
}

// ============================================================================
// Hot-recompile tests
//
// Verifies that recompiling after a dependency file changes picks up the new
// content (regression for the libGroup module cache bug).
// ============================================================================

TEST_SUITE("GameScript Hot Recompile")
{
    TEST_CASE("Recompile after dependency change picks up new content")
    {
        // First version of the dependency exports dep_fn_v1 but not dep_fn_v2.
        WriteDas("_test_hotreload_dep.das",
            "options gen2\n"
            "[export] def dep_fn_v1() {}\n");

        auto mainPath = WriteDas("_test_hotreload_main.das",
            "options gen2\n"
            "require _test_hotreload_dep\n"
            "[export] def game_ready() {}\n"
            "[export] def game_update() {}\n"
            "[export] def game_physics_update() {}\n"
            "[export] def game_draw() {}\n"
            "[export] def game_draw_ui() {}\n");

        duin::GameScript gs(mainPath);
        gs.InitModules([]() {
            NEED_MODULE(Module_DnLog); NEED_MODULE(Module_DnGameObject);
        });

        REQUIRE(gs.CompileAndSimulate(true));
        CHECK(gs.FindFunction("dep_fn_v1") != nullptr);
        CHECK(gs.FindFunction("dep_fn_v2") == nullptr);

        // Overwrite the dependency with a new function
        WriteDas("_test_hotreload_dep.das",
            "options gen2\n"
            "[export] def dep_fn_v1() {}\n"
            "[export] def dep_fn_v2() {}\n");

        // Recompile — must pick up the updated dep, not the cached module
        REQUIRE(gs.CompileAndSimulate(true));
        CHECK(gs.FindFunction("dep_fn_v1") != nullptr);
        CHECK(gs.FindFunction("dep_fn_v2") != nullptr);

        gs.ResetScript();
    }

    TEST_CASE("Recompile after main script change picks up new exported function")
    {
        auto path = WriteDas("_test_hotreload_mainfn.das",
            "options gen2\n"
            "[export] def game_ready() {}\n"
            "[export] def game_update() {}\n"
            "[export] def game_physics_update() {}\n"
            "[export] def game_draw() {}\n"
            "[export] def game_draw_ui() {}\n");

        duin::GameScript gs(path);
        gs.InitModules([]() {
            NEED_MODULE(Module_DnLog); NEED_MODULE(Module_DnGameObject);
        });

        REQUIRE(gs.CompileAndSimulate(true));
        CHECK(gs.FindFunction("new_fn") == nullptr);

        // Rewrite script adding a new function
        WriteDas("_test_hotreload_mainfn.das",
            "options gen2\n"
            "[export] def game_ready() {}\n"
            "[export] def game_update() {}\n"
            "[export] def game_physics_update() {}\n"
            "[export] def game_draw() {}\n"
            "[export] def game_draw_ui() {}\n"
            "[export] def new_fn() {}\n");

        REQUIRE(gs.CompileAndSimulate(true));
        CHECK(gs.FindFunction("new_fn") != nullptr);

        gs.ResetScript();
    }
}

} // namespace TestGameScript
