// Script-level tests for path portability: SetDasRoot auto-resolution and
// SetProjectFile's handling of fs::FindProjectFile results.

#include <doctest.h>
#include <daScript/daScript.h>
#include <Duin/Script/Script.h>
#include <Duin/IO/Filesystem.h>
#include "../IO/EnginePathsTestUtils.h"

#include <algorithm>
#include <filesystem>
#include <string>

using namespace EnginePathsTest;

namespace TestScriptPaths
{

// Exposes the protected project file for inspection
class ScriptProbe : public duin::Script
{
  public:
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

} // namespace TestScriptPaths
