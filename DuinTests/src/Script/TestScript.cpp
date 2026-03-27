#include <doctest.h>
#include <Duin/Script/Script.h>

static const std::string ARTIFACTS_DIR = "./artifacts";

// Helper: write a .das file under ARTIFACTS_DIR and return its path.
static std::string WriteDas(const std::string &name, const std::string &src)
{
    std::string path = ARTIFACTS_DIR + "/" + name;
    FILE *f = fopen(path.c_str(), "w");
    if (f) { fwrite(src.data(), 1, src.size(), f); fclose(f); }
    return path;
}

namespace TestScript
{

TEST_SUITE("Script")
{
    TEST_CASE("Compile succeeds for a valid daslang script")
    {
        auto path = WriteDas("_test_script_valid.das",
            "options gen2\n"
            "[export] def main() {}\n");

        duin::Script script(path);
        script.InitModules();
        bool compiled = script.Compile();
        script.Exit();

        CHECK(compiled);
    }

    TEST_CASE("Compile fails for a script with a syntax error")
    {
        auto path = WriteDas("_test_script_invalid.das",
            "options gen2\n"
            "def main() { this is not valid daslang }\n");

        duin::Script script(path);
        script.InitModules();
        bool compiled = script.Compile();
        script.Exit();

        CHECK_FALSE(compiled);
    }

    TEST_CASE("SimulateContext succeeds after a successful Compile")
    {
        auto path = WriteDas("_test_script_simulate.das",
            "options gen2\n"
            "[export] def main() {}\n");

        duin::Script script(path);
        script.InitModules();
        script.Compile();
        bool simulated = script.SimulateContext();
        script.Exit();

        CHECK(simulated);
    }

    TEST_CASE("FindFunction returns non-null for an exported function")
    {
        auto path = WriteDas("_test_script_find.das",
            "options gen2\n"
            "[export] def my_func() {}\n");

        duin::Script script(path);
        script.InitModules();
        script.Compile();
        script.SimulateContext();

        das::SimFunction *fn = script.FindFunction("my_func");
        script.Exit();

        CHECK(fn != nullptr);
    }

    TEST_CASE("FindFunction returns null for a non-existent function")
    {
        auto path = WriteDas("_test_script_find_missing.das",
            "options gen2\n"
            "[export] def real_func() {}\n");

        duin::Script script(path);
        script.InitModules();
        script.Compile();
        script.SimulateContext();

        das::SimFunction *fn = script.FindFunction("does_not_exist");
        script.Exit();

        CHECK(fn == nullptr);
    }

    TEST_CASE("CallScript executes an exported function without throwing")
    {
        auto path = WriteDas("_test_script_call.das",
            "options gen2\n"
            "[export] def do_work() {}\n");

        duin::Script script(path);
        script.InitModules();
        script.Compile();
        script.SimulateContext();

        das::SimFunction *fn = script.FindFunction("do_work");
        bool called = script.CallScript(fn);
        script.Exit();

        CHECK(called);
    }

    TEST_CASE("CallScript returns false when function is null")
    {
        auto path = WriteDas("_test_script_call_null.das",
            "options gen2\n"
            "[export] def main() {}\n");

        duin::Script script(path);
        script.InitModules();
        script.Compile();
        script.SimulateContext();

        bool result = script.CallScript(nullptr);
        script.Exit();

        CHECK_FALSE(result);
    }
}

} // namespace TestScript
