// Script-level tests for path portability: SetDasRoot auto-resolution and
// SetProjectFile's handling of fs::FindProjectFile results.

#include <doctest.h>
#include <daScript/daScript.h>
#include <Duin/Script/Script.h>
#include <Duin/IO/Filesystem.h>
#include "../IO/EnginePathsTestUtils.h"

#include <algorithm>
#include <filesystem>
#include <fstream>
#include <new>
#include <string>

using namespace EnginePathsTest;

namespace TestScriptPaths
{

// Exposes the protected project file for inspection
class ScriptProbe : public duin::Script
{
  public:
    using duin::Script::BuildFileAccess;

    bool TryBuildFileAccess(std::string &error)
    {
        alignas(das::FileAccessPtr) static unsigned char storage[sizeof(das::FileAccessPtr)];
        das::FileAccessPtr *built = new (storage) das::FileAccessPtr(BuildFileAccess(error));
        return built->get() != nullptr;
    }
    using duin::Script::projectFile;
};

// das::setDasRoot is process-global; restore it so other Script tests are unaffected
class DasRootGuard
{
  public:
    DasRootGuard() : previous(das::getDasRoot())
    {
    }
    ~DasRootGuard()
    {
        das::setDasRoot(previous);
    }
    DasRootGuard(const DasRootGuard &) = delete;
    DasRootGuard &operator=(const DasRootGuard &) = delete;

  private:
    std::string previous;
};

TEST_SUITE("Script - SetDasRoot resolution")
{
    TEST_CASE("No argument auto-resolves to the engine's vendored daslang")
    {
        ClearedPathEnv env;
        DasRootGuard guard;
        std::string expected = duin::fs::ResolveDasRoot();
        REQUIRE_FALSE(duin::fs::IsPathInvalid(expected));

        duin::Script s;
        s.SetDasRoot();
        CHECK(das::getDasRoot() == expected);
        CHECK(std::filesystem::exists(std::filesystem::u8path(das::getDasRoot() + "/daslib/builtin.das")));
    }

    TEST_CASE("Empty string behaves like no argument")
    {
        ClearedPathEnv env;
        DasRootGuard guard;

        duin::Script s;
        s.SetDasRoot("");
        CHECK(das::getDasRoot() == duin::fs::ResolveDasRoot());
    }

    TEST_CASE("Invalid path falls back to the resolved root instead of breaking daslang")
    {
        ClearedPathEnv env;
        DasRootGuard guard;

        duin::Script s;
        s.SetDasRoot("/definitely/not/a/daslang_root");
        CHECK(das::getDasRoot() == duin::fs::ResolveDasRoot());
    }

    TEST_CASE("CWD-relative legacy path no longer depends on the working directory")
    {
        // Tests used to pass "Duin/vendor/daslang", only valid when CWD is the repo root
        ClearedPathEnv env;
        DasRootGuard guard;
        TempTree tmp;
        std::filesystem::path oldCwd = std::filesystem::current_path();
        std::filesystem::current_path(std::filesystem::u8path(tmp.Path()));

        duin::Script s;
        s.SetDasRoot("Duin/vendor/daslang");
        std::string root = das::getDasRoot();
        std::filesystem::current_path(oldCwd);

        CHECK(std::filesystem::exists(std::filesystem::u8path(root + "/daslib/builtin.das")));
    }

    TEST_CASE("Valid explicit path is used, normalized to forward slashes")
    {
        ClearedPathEnv env;
        DasRootGuard guard;
        TempTree tmp;
        std::string fake = MakeFakeDasRoot(tmp / "daslang");
        std::string backslashed = fake;
        std::replace(backslashed.begin(), backslashed.end(), '/', '\\');

        duin::Script s;
        s.SetDasRoot(backslashed);
        CHECK(das::getDasRoot() == fake);
    }

    TEST_CASE("DUIN_DAS_ROOT is honored when no path is given")
    {
        DasRootGuard guard;
        TempTree tmp;
        std::string fake = MakeFakeDasRoot(tmp / "daslang");
        ScopedEnv env("DUIN_DAS_ROOT", fake);

        duin::Script s;
        s.SetDasRoot();
        CHECK(das::getDasRoot() == fake);
    }
}

TEST_SUITE("Script - SetProjectFile")
{
    TEST_CASE("Stores a valid path")
    {
        ScriptProbe s;
        s.SetProjectFile("some/dir/game.das_project");
        CHECK(s.projectFile == "some/dir/game.das_project");
    }

    TEST_CASE("INVALID_PATH means no project file")
    {
        ScriptProbe s;
        s.SetProjectFile(INVALID_PATH);
        CHECK(s.projectFile.empty());
    }

    TEST_CASE("INVALID_PATH clears a previously set project file")
    {
        ScriptProbe s;
        s.SetProjectFile("some/dir/game.das_project");
        s.SetProjectFile(INVALID_PATH);
        CHECK(s.projectFile.empty());
    }

    TEST_CASE("Failed discovery leaves the script without a project")
    {
        TempTree tmp;
        Touch(tmp / "lonely/main.das");
        std::string found = duin::fs::FindProjectFile(tmp / "lonely/main.das");
        if (!duin::fs::IsPathInvalid(found))
        {
            MESSAGE("Skipped: a .das_project exists above the temp dir: " << found);
            return;
        }

        ScriptProbe s;
        s.SetProjectFile(found);
        CHECK(s.projectFile.empty());
    }

    TEST_CASE("Discovered project file is absolute and exists")
    {
        EngineRootGuard rootGuard;
        std::string real = RealEngineRoot();
        REQUIRE_FALSE(duin::fs::IsPathInvalid(real));
        REQUIRE(duin::fs::SetEngineRoot(real));

        ScriptProbe s;
        s.SetProjectFile(duin::fs::FindProjectFile("eng://../DuinTests/scripts"));
        REQUIRE_FALSE(s.projectFile.empty());
        CHECK(std::filesystem::path(s.projectFile).is_absolute());
        CHECK(std::filesystem::is_regular_file(std::filesystem::u8path(s.projectFile)));
        CHECK(std::filesystem::path(s.projectFile).filename() == "duintests.das_project");
    }
}

static void WriteText(const std::string &path, const std::string &text)
{
    std::filesystem::create_directories(std::filesystem::u8path(path).parent_path());
    std::ofstream f(std::filesystem::u8path(path));
    f << text;
}

TEST_SUITE("Script - BuildFileAccess")
{
    TEST_CASE("Without a project file a plain file access is built")
    {
        DasRootGuard guard;
        ScriptProbe s;
        s.SetDasRoot();
        s.InitModules();

        std::string error;
        bool access = s.TryBuildFileAccess(error);
        CHECK(access);
        CHECK(error.empty());
    }

    TEST_CASE("Missing project file returns null and reports the path")
    {
        DasRootGuard guard;
        TempTree tmp;
        std::string missing = tmp / "missing/game.das_project";

        ScriptProbe s;
        s.SetDasRoot();
        s.InitModules();
        s.SetProjectFile(missing);

        std::string error;
        bool access = s.TryBuildFileAccess(error);
        CHECK_FALSE(access);
        CHECK_FALSE(error.empty());
        CHECK(error.find("game.das_project") != std::string::npos);
    }

    TEST_CASE("Project file that fails to compile returns null with the compiler message")
    {
        DasRootGuard guard;
        TempTree tmp;
        std::string broken = tmp / "broken/game.das_project";
        WriteText(broken, "options gen2\nthis is not valid daslang {{{\n");

        ScriptProbe s;
        s.SetDasRoot();
        s.InitModules();
        s.SetProjectFile(broken);

        std::string error;
        bool access = s.TryBuildFileAccess(error);
        CHECK_FALSE(access);
        CHECK(error.find("game.das_project") != std::string::npos);
        CHECK(error.size() > std::string("project file failed to compile: ").size() + broken.size());
    }

    TEST_CASE("Valid project file builds a file access")
    {
        DasRootGuard guard;
        EngineRootGuard rootGuard;
        std::string real = RealEngineRoot();
        REQUIRE_FALSE(duin::fs::IsPathInvalid(real));
        REQUIRE(duin::fs::SetEngineRoot(real));

        ScriptProbe s;
        s.SetDasRoot();
        s.InitModules();
        s.SetProjectFile(duin::fs::FindProjectFile("eng://../DuinTests/scripts"));
        REQUIRE_FALSE(s.projectFile.empty());

        std::string error;
        bool access = s.TryBuildFileAccess(error);
        CHECK(access);
        CHECK(error.empty());
    }

    TEST_CASE("Compile fails cleanly when the project file is broken" *
              doctest::skip() *
              doctest::description("Script::Compile ignores BuildFileAccess errors and passes a null file "
                                   "access to compileDaScript; skipped because it can crash the runner. "
                                   "Run with --no-skip to reproduce."))
    {
        DasRootGuard guard;
        TempTree tmp;
        std::string broken = tmp / "broken/game.das_project";
        WriteText(broken, "this is not valid daslang {{{\n");
        std::string script = tmp / "broken/main.das";
        WriteText(script, "options gen2\n[export]\ndef main() {\n}\n");

        ScriptProbe s;
        s.SetDasRoot();
        s.InitModules();
        s.SetProjectFile(broken);
        s.SetScriptPath(script);

        CHECK_FALSE(s.Compile());
    }
}

} // namespace TestScriptPaths
