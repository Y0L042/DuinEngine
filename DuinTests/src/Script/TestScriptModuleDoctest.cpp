#include <doctest.h>
#include <Duin/Script/Script.h>
#include <Duin/Script/DNLog/Module_DnLog.h>
#include <Duin/Script/Doctest/Module_Doctest.h>

#if 0

namespace TestScriptModuleDoctest
{

static const std::string SCRIPTS_DIR = "./scripts/Doctest";

TEST_SUITE("ScriptModuleDoctest")
{
    TEST_CASE("dn_check passes when condition is true")
    {
        duin::Script script(SCRIPTS_DIR + "/test_dn_check.das");
        script.InitModules([]() { NEED_MODULE(Module_DnLog); NEED_MODULE(Module_Doctest); });
        REQUIRE(script.Compile());
        REQUIRE(script.SimulateContext());
        CHECK(script.CallScript(script.FindFunction("test_dn_check")));
        script.Exit();
    }

    TEST_CASE("dn_check_false passes when condition is false")
    {
        duin::Script script(SCRIPTS_DIR + "/test_dn_check_false.das");
        script.InitModules([]() { NEED_MODULE(Module_DnLog); NEED_MODULE(Module_Doctest); });
        REQUIRE(script.Compile());
        REQUIRE(script.SimulateContext());
        CHECK(script.CallScript(script.FindFunction("test_dn_check_false")));
        script.Exit();
    }

    TEST_CASE("dn_require passes when condition is true")
    {
        duin::Script script(SCRIPTS_DIR + "/test_dn_require.das");
        script.InitModules([]() { NEED_MODULE(Module_DnLog); NEED_MODULE(Module_Doctest); });
        REQUIRE(script.Compile());
        REQUIRE(script.SimulateContext());
        CHECK(script.CallScript(script.FindFunction("test_dn_require")));
        script.Exit();
    }

    TEST_CASE("dn_require_false passes when condition is false")
    {
        duin::Script script(SCRIPTS_DIR + "/test_dn_require_false.das");
        script.InitModules([]() { NEED_MODULE(Module_DnLog); NEED_MODULE(Module_Doctest); });
        REQUIRE(script.Compile());
        REQUIRE(script.SimulateContext());
        CHECK(script.CallScript(script.FindFunction("test_dn_require_false")));
        script.Exit();
    }

    TEST_CASE("dn_message never fails")
    {
        duin::Script script(SCRIPTS_DIR + "/test_dn_message.das");
        script.InitModules([]() { NEED_MODULE(Module_DnLog); NEED_MODULE(Module_Doctest); });
        REQUIRE(script.Compile());
        REQUIRE(script.SimulateContext());
        CHECK(script.CallScript(script.FindFunction("test_dn_message")));
        script.Exit();
    }

    TEST_CASE("dn_fail module loads and symbol is exported")
    {
        duin::Script script(SCRIPTS_DIR + "/test_dn_fail.das");
        script.InitModules([]() { NEED_MODULE(Module_DnLog); NEED_MODULE(Module_Doctest); });
        REQUIRE(script.Compile());
        REQUIRE(script.SimulateContext());
        CHECK(script.CallScript(script.FindFunction("test_dn_fail_available")));
        script.Exit();
    }

    TEST_CASE("dn_fail_check module loads and symbol is exported")
    {
        duin::Script script(SCRIPTS_DIR + "/test_dn_fail_check.das");
        script.InitModules([]() { NEED_MODULE(Module_DnLog); NEED_MODULE(Module_Doctest); });
        REQUIRE(script.Compile());
        REQUIRE(script.SimulateContext());
        CHECK(script.CallScript(script.FindFunction("test_dn_fail_check_available")));
        script.Exit();
    }

    TEST_CASE("multiple dn_doctest assertions in one script all pass")
    {
        duin::Script script(SCRIPTS_DIR + "/test_dn_combined.das");
        script.InitModules([]() { NEED_MODULE(Module_DnLog); NEED_MODULE(Module_Doctest); });
        REQUIRE(script.Compile());
        REQUIRE(script.SimulateContext());
        CHECK(script.CallScript(script.FindFunction("test_combined")));
        script.Exit();
    }
}

} // namespace TestScriptModuleDoctest

#endif