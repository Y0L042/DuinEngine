#include <doctest.h>
#include <Duin/Script/GameScript.h>
#include <Duin/Script/DNLog/Module_DnLog.h>
#include <Duin/Script/GameObject/Module_DnGameObject.h>

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

        gs.Exit();
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

        gs.Exit();
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
        gs.Exit();
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

        gs.Exit();
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

        gs.Exit();
    }

    TEST_CASE("Exit can be called multiple times without crashing")
    {
        auto path = WriteDas("_test_gamescript_exit.das",
            "options gen2\n"
            "[export] def game_ready() {}\n");

        duin::GameScript gs(path);
        gs.InitModules([]() {
            NEED_MODULE(Module_DnLog); NEED_MODULE(Module_DnGameObject);
        });
        gs.CompileAndSimulate();

        CHECK_NOTHROW(gs.Exit());
        CHECK_NOTHROW(gs.Exit());
    }
}

} // namespace TestGameScript
