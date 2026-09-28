// Tests for machine-independent path resolution: engine root, daslang root, the eng://
// virtual prefix, and *.das_project discovery. These replace the hard-coded absolute
// paths the engine and apps used to carry (see duin::fs::ResolveEngineRoot & co.).

#include <doctest.h>
#include <Duin/IO/Filesystem.h>
#include "EnginePathsTestUtils.h"

#include <algorithm>
#include <cctype>
#include <filesystem>
#include <fstream>
#include <regex>
#include <string>
#include <vector>

using namespace EnginePathsTest;

namespace TestEnginePaths
{

static std::string ToBackslashes(std::string path)
{
    std::replace(path.begin(), path.end(), '/', '\\');
    return path;
}

static bool HasEngineMarker(const std::string &engineRoot)
{
    return std::filesystem::exists(std::filesystem::u8path(engineRoot + "/vendor/daslang/daslib/builtin.das"));
}

// =============================================================================
//  ResolveEngineRoot
// =============================================================================

TEST_SUITE("EnginePaths - ResolveEngineRoot")
{
    TEST_CASE("Accepts a valid override verbatim")
    {
        ClearedPathEnv env;
        TempTree tmp;
        std::string fake = MakeFakeEngineRoot(tmp.Path());

        CHECK(duin::fs::ResolveEngineRoot(fake) == fake);
    }

    TEST_CASE("Strips trailing slashes from the override")
    {
        ClearedPathEnv env;
        TempTree tmp;
        std::string fake = MakeFakeEngineRoot(tmp.Path());

        CHECK(duin::fs::ResolveEngineRoot(fake + "/") == fake);
        CHECK(duin::fs::ResolveEngineRoot(fake + "//") == fake);
    }

    TEST_CASE("Normalizes a backslashed override to forward slashes")
    {
        ClearedPathEnv env;
        TempTree tmp;
        std::string fake = MakeFakeEngineRoot(tmp.Path());

        std::string resolved = duin::fs::ResolveEngineRoot(ToBackslashes(fake));
        CHECK(resolved == fake);
        CHECK(resolved.find('\\') == std::string::npos);
    }

    TEST_CASE("Makes a relative override absolute")
    {
        ClearedPathEnv env;
        TempTree tmp("./artifacts");
        std::string fake = MakeFakeEngineRoot(tmp.Path());
        std::string relative =
            std::filesystem::relative(std::filesystem::u8path(fake), std::filesystem::current_path()).generic_string();
        REQUIRE_FALSE(std::filesystem::path(relative).is_absolute());

        CHECK(duin::fs::ResolveEngineRoot(relative) == fake);
    }

    TEST_CASE("Collapses dot segments in the override")
    {
        ClearedPathEnv env;
        TempTree tmp;
        std::string fake = MakeFakeEngineRoot(tmp.Path());

        CHECK(duin::fs::ResolveEngineRoot(fake + "/vendor/../") == fake);
    }

    TEST_CASE("Rejects an override without the daslang marker")
    {
        ClearedPathEnv env;
        TempTree tmp;
        std::filesystem::create_directories(tmp / "Duin/vendor/daslang/daslib"); // no builtin.das

        std::string resolved = duin::fs::ResolveEngineRoot(tmp / "Duin");
        CHECK(resolved != Norm(tmp / "Duin"));
    }

    TEST_CASE("Rejects a das root passed as an engine root")
    {
        ClearedPathEnv env;
        TempTree tmp;
        std::string dasRoot = MakeFakeDasRoot(tmp / "daslang");

        CHECK(duin::fs::ResolveEngineRoot(dasRoot) != dasRoot);
    }

    TEST_CASE("Rejects a non-existent override")
    {
        ClearedPathEnv env;
        std::string resolved = duin::fs::ResolveEngineRoot("Z:/definitely/not/here");
        CHECK(resolved.find("definitely") == std::string::npos);
    }

    TEST_CASE("Honors DUIN_ROOT when no override is given")
    {
        TempTree tmp;
        std::string fake = MakeFakeEngineRoot(tmp.Path());
        ScopedEnv env("DUIN_ROOT", fake);

        CHECK(duin::fs::ResolveEngineRoot() == fake);
    }

    TEST_CASE("Override takes priority over DUIN_ROOT")
    {
        TempTree tmpEnv;
        TempTree tmpOverride;
        std::string envRoot = MakeFakeEngineRoot(tmpEnv.Path());
        std::string overrideRoot = MakeFakeEngineRoot(tmpOverride.Path());
        ScopedEnv env("DUIN_ROOT", envRoot);

        CHECK(duin::fs::ResolveEngineRoot(overrideRoot) == overrideRoot);
    }

    TEST_CASE("Invalid override falls through to DUIN_ROOT")
    {
        TempTree tmp;
        std::string fake = MakeFakeEngineRoot(tmp.Path());
        ScopedEnv env("DUIN_ROOT", fake);

        CHECK(duin::fs::ResolveEngineRoot(tmp / "nope") == fake);
    }

    TEST_CASE("Invalid DUIN_ROOT falls through to walk-up")
    {
        ScopedEnv env("DUIN_ROOT", std::string("/c/msys/style/path"));
        ScopedEnv dasEnv("DUIN_DAS_ROOT", std::nullopt);

        std::string resolved = duin::fs::ResolveEngineRoot();
        REQUIRE_FALSE(duin::fs::IsPathInvalid(resolved));
        CHECK(resolved.find("msys") == std::string::npos);
        CHECK(HasEngineMarker(resolved));
    }

    TEST_CASE("Walk-up finds the real engine root of this repo")
    {
        std::string root = RealEngineRoot();
        REQUIRE_FALSE(duin::fs::IsPathInvalid(root));

        CHECK(std::filesystem::path(root).is_absolute());
        CHECK(root.find('\\') == std::string::npos);
        CHECK(root.back() != '/');
        CHECK(std::filesystem::path(root).filename() == "Duin");
        CHECK(HasEngineMarker(root));
        CHECK(std::filesystem::exists(root + "/src/Duin"));
    }

    TEST_CASE("Walk-up is independent of the current working directory")
    {
        std::string before = RealEngineRoot();
        TempTree tmp;
        std::filesystem::path oldCwd = std::filesystem::current_path();
        std::filesystem::current_path(std::filesystem::u8path(tmp.Path()));

        std::string after = RealEngineRoot();
        std::filesystem::current_path(oldCwd);

        CHECK(after == before);
    }

    TEST_CASE("Resolution is stable across calls")
    {
        CHECK(RealEngineRoot() == RealEngineRoot());
    }
}

// =============================================================================
//  ResolveDasRoot
// =============================================================================

TEST_SUITE("EnginePaths - ResolveDasRoot")
{
    TEST_CASE("Accepts a valid override")
    {
        ClearedPathEnv env;
        TempTree tmp;
        std::string dasRoot = MakeFakeDasRoot(tmp / "daslang");

        CHECK(duin::fs::ResolveDasRoot(dasRoot) == dasRoot);
    }

    TEST_CASE("Normalizes backslashes and trailing slashes in the override")
    {
        ClearedPathEnv env;
        TempTree tmp;
        std::string dasRoot = MakeFakeDasRoot(tmp / "daslang");

        CHECK(duin::fs::ResolveDasRoot(ToBackslashes(dasRoot) + "\\") == dasRoot);
    }

    TEST_CASE("Rejects MSYS-style paths the native file layer cannot open")
    {
        ClearedPathEnv env;
        std::string resolved = duin::fs::ResolveDasRoot("/c/Projects/Duin/Duin/vendor/daslang");
        CHECK(resolved.rfind("/c/", 0) != 0);
    }

    TEST_CASE("Rejects a directory without daslib/builtin.das")
    {
        ClearedPathEnv env;
        TempTree tmp;
        std::filesystem::create_directories(tmp / "daslang/daslib");

        CHECK(duin::fs::ResolveDasRoot(tmp / "daslang") != Norm(tmp / "daslang"));
    }

    TEST_CASE("Honors DUIN_DAS_ROOT when no override is given")
    {
        TempTree tmp;
        std::string dasRoot = MakeFakeDasRoot(tmp / "daslang");
        ScopedEnv env("DUIN_DAS_ROOT", dasRoot);

        CHECK(duin::fs::ResolveDasRoot() == dasRoot);
    }

    TEST_CASE("Override takes priority over DUIN_DAS_ROOT")
    {
        TempTree tmp;
        std::string envRoot = MakeFakeDasRoot(tmp / "env_daslang");
        std::string overrideRoot = MakeFakeDasRoot(tmp / "override_daslang");
        ScopedEnv env("DUIN_DAS_ROOT", envRoot);

        CHECK(duin::fs::ResolveDasRoot(overrideRoot) == overrideRoot);
    }

    TEST_CASE("Invalid DUIN_DAS_ROOT falls through to the engine's vendored daslang")
    {
        ScopedEnv env("DUIN_DAS_ROOT", std::string("/c/not/a/das/root"));
        std::string engineRoot = duin::fs::GetEngineRoot();
        REQUIRE_FALSE(duin::fs::IsPathInvalid(engineRoot));

        CHECK(duin::fs::ResolveDasRoot() == engineRoot + "vendor/daslang");
    }

    TEST_CASE("Default is <engine root>/vendor/daslang")
    {
        ClearedPathEnv env;
        std::string engineRoot = duin::fs::GetEngineRoot();
        REQUIRE_FALSE(duin::fs::IsPathInvalid(engineRoot));

        std::string dasRoot = duin::fs::ResolveDasRoot();
        CHECK(dasRoot == engineRoot + "vendor/daslang");
        CHECK(std::filesystem::exists(std::filesystem::u8path(dasRoot + "/daslib/builtin.das")));
    }

    TEST_CASE("Default follows SetEngineRoot")
    {
        ClearedPathEnv env;
        EngineRootGuard guard;
        TempTree tmp;
        std::string fake = MakeFakeEngineRoot(tmp.Path());
        REQUIRE(duin::fs::SetEngineRoot(fake));

        CHECK(duin::fs::ResolveDasRoot() == fake + "/vendor/daslang");
    }

    TEST_CASE("Real das root ships the Debug JIT runtime library")
    {
        // Script.cpp builds jit_path_to_shared_lib from the das root in Debug builds
        ClearedPathEnv env;
        std::string dasRoot = duin::fs::ResolveDasRoot();
        REQUIRE_FALSE(duin::fs::IsPathInvalid(dasRoot));

        CHECK(std::filesystem::exists(std::filesystem::u8path(dasRoot + "/lib/Debug/libDaScriptDyn_runtime.lib")));
    }
}

// =============================================================================
//  SetEngineRoot / GetEngineRoot
// =============================================================================

TEST_SUITE("EnginePaths - SetEngineRoot and GetEngineRoot")
{
    TEST_CASE("Rejects an invalid root and keeps the previous one")
    {
        EngineRootGuard guard;
        std::string before = duin::fs::GetEngineRoot();
        TempTree tmp;

        CHECK_FALSE(duin::fs::SetEngineRoot(tmp.Path()));
        CHECK(duin::fs::GetEngineRoot() == before);
    }

    TEST_CASE("Rejects an empty root")
    {
        EngineRootGuard guard;
        std::string before = duin::fs::GetEngineRoot();

        CHECK_FALSE(duin::fs::SetEngineRoot(""));
        CHECK(duin::fs::GetEngineRoot() == before);
    }

    TEST_CASE("Accepts a valid root and stores it with a trailing slash")
    {
        EngineRootGuard guard;
        TempTree tmp;
        std::string fake = MakeFakeEngineRoot(tmp.Path());

        REQUIRE(duin::fs::SetEngineRoot(fake));
        CHECK(duin::fs::GetEngineRoot() == fake + "/");
    }

    TEST_CASE("Normalizes backslashes and trailing slashes")
    {
        EngineRootGuard guard;
        TempTree tmp;
        std::string fake = MakeFakeEngineRoot(tmp.Path());

        REQUIRE(duin::fs::SetEngineRoot(ToBackslashes(fake) + "\\"));
        CHECK(duin::fs::GetEngineRoot() == fake + "/");
    }

    TEST_CASE("Guard restores the real engine root")
    {
        std::string real = RealEngineRoot();
        REQUIRE_FALSE(duin::fs::IsPathInvalid(real));
        REQUIRE(duin::fs::SetEngineRoot(real));
        {
            EngineRootGuard guard;
            TempTree tmp;
            REQUIRE(duin::fs::SetEngineRoot(MakeFakeEngineRoot(tmp.Path())));
        }
        CHECK(duin::fs::GetEngineRoot() == real + "/");
    }
}

// =============================================================================
//  eng:// virtual prefix
// =============================================================================

TEST_SUITE("EnginePaths - eng:// virtual prefix")
{
    TEST_CASE("eng:// is recognized as virtual, near-misses are not")
    {
        CHECK(duin::fs::IsVirtualPath("eng://src/file.bin"));
        CHECK(duin::fs::IsVirtualPath("eng://"));
        CHECK_FALSE(duin::fs::IsVirtualPath("eng:/src/file.bin"));
        CHECK_FALSE(duin::fs::IsVirtualPath("eng:src/file.bin"));
        CHECK_FALSE(duin::fs::IsVirtualPath("ENG://src/file.bin"));
        CHECK_FALSE(duin::fs::IsVirtualPath("engine://src/file.bin"));
    }

    TEST_CASE("Maps eng:// paths onto the engine root")
    {
        EngineRootGuard guard;
        TempTree tmp;
        std::string fake = MakeFakeEngineRoot(tmp.Path());
        REQUIRE(duin::fs::SetEngineRoot(fake));

        CHECK(duin::fs::MapVirtualToSystemPath("eng://src/Duin/Resources/a.bin") == fake + "/src/Duin/Resources/a.bin");
        CHECK(duin::fs::MapVirtualToSystemPath("eng://") == fake + "/");
    }

    TEST_CASE("Keeps .. segments so repo siblings are reachable (editor config)")
    {
        EngineRootGuard guard;
        TempTree tmp;
        std::string fake = MakeFakeEngineRoot(tmp.Path());
        REQUIRE(duin::fs::SetEngineRoot(fake));
        std::filesystem::create_directories(tmp / "ExampleProjects/Game");

        std::string mapped = duin::fs::MapVirtualToSystemPath("eng://../ExampleProjects/Game");
        CHECK(mapped == fake + "/../ExampleProjects/Game");
        CHECK(std::filesystem::is_directory(std::filesystem::u8path(mapped)));
    }

    TEST_CASE("System paths under the engine root map back to eng://")
    {
        EngineRootGuard guard;
        TempTree tmp;
        std::string fake = MakeFakeEngineRoot(tmp.Path());
        REQUIRE(duin::fs::SetEngineRoot(fake));
        duin::fs::SetWorkspacePath(tmp / "unrelated_workspace");

        CHECK(duin::fs::MapSystemToVirtualPath(fake + "/src/x.bin") == "eng://src/x.bin");
        CHECK(duin::fs::MapSystemToVirtualPath(ToBackslashes(fake + "/src/x.bin")) == "eng://src/x.bin");
    }

    TEST_CASE("Round-trips virtual -> system -> virtual")
    {
        EngineRootGuard guard;
        TempTree tmp;
        REQUIRE(duin::fs::SetEngineRoot(MakeFakeEngineRoot(tmp.Path())));
        duin::fs::SetWorkspacePath(tmp / "unrelated_workspace");

        const std::string original = "eng://src/Duin/Resources/shaders/dx11/vs_cubes.bin";
        CHECK(duin::fs::MapSystemToVirtualPath(duin::fs::MapVirtualToSystemPath(original)) == original);
    }

    TEST_CASE("Sibling directories sharing the root's name prefix are not eng://")
    {
        // Root ".../Duin/" must not claim ".../DuinTests/..." — the trailing slash matters
        EngineRootGuard guard;
        TempTree tmp;
        std::string fake = MakeFakeEngineRoot(tmp.Path());
        REQUIRE(duin::fs::SetEngineRoot(fake));
        duin::fs::SetWorkspacePath(tmp / "unrelated_workspace");

        std::string sibling = tmp / "DuinTests/file.txt";
        CHECK(duin::fs::MapSystemToVirtualPath(sibling).rfind("eng://", 0) != 0);
    }

    TEST_CASE("wrk:// takes priority when the workspace is inside the engine root")
    {
        EngineRootGuard guard;
        TempTree tmp;
        std::string fake = MakeFakeEngineRoot(tmp.Path());
        REQUIRE(duin::fs::SetEngineRoot(fake));
        duin::fs::SetWorkspacePath(fake + "/src");

        CHECK(duin::fs::MapSystemToVirtualPath(fake + "/src/a.das") == "wrk://a.das");
        CHECK(duin::fs::MapSystemToVirtualPath(fake + "/vendor/b.das") == "eng://vendor/b.das");
    }

    TEST_CASE("Default renderer shaders resolve through eng://")
    {
        // Renderer.cpp loads these at InitRenderer(); they used to be absolute paths
        EngineRootGuard guard;
        std::string real = RealEngineRoot();
        REQUIRE_FALSE(duin::fs::IsPathInvalid(real));
        REQUIRE(duin::fs::SetEngineRoot(real));

        for (const char *shader : {"eng://src/Duin/Resources/shaders/dx11/vs_cubes.bin",
                                   "eng://src/Duin/Resources/shaders/dx11/fs_cubes.bin"})
        {
            std::string sys = duin::fs::MapVirtualToSystemPath(shader);
            INFO("shader: " << shader << " -> " << sys);
            CHECK(std::filesystem::is_regular_file(std::filesystem::u8path(sys)));
        }
    }

    TEST_CASE("Editor config project dirs resolve through eng://")
    {
        EngineRootGuard guard;
        std::string real = RealEngineRoot();
        REQUIRE_FALSE(duin::fs::IsPathInvalid(real));
        REQUIRE(duin::fs::SetEngineRoot(real));

        for (const char *dir : {"eng://../ExampleProjects/DuinFPS", "eng://../ExampleProjects/DuinFPSDaslang",
                                "eng://../ExampleProjects/Sandbox"})
        {
            std::string sys = duin::fs::MapVirtualToSystemPath(dir);
            INFO("dir: " << dir << " -> " << sys);
            CHECK(std::filesystem::is_directory(std::filesystem::u8path(sys)));
        }
    }
}

// =============================================================================
//  FindProjectFile
// =============================================================================

TEST_SUITE("EnginePaths - FindProjectFile")
{
    TEST_CASE("Finds a project file next to the script")
    {
        TempTree tmp;
        Touch(tmp / "proj/game.das_project");
        Touch(tmp / "proj/main.das");

        CHECK(duin::fs::FindProjectFile(tmp / "proj/main.das") == Norm(tmp / "proj/game.das_project"));
    }

    TEST_CASE("Walks up from a nested script")
    {
        TempTree tmp;
        Touch(tmp / "proj/game.das_project");
        Touch(tmp / "proj/scripts/a/b/main.das");

        CHECK(duin::fs::FindProjectFile(tmp / "proj/scripts/a/b/main.das") == Norm(tmp / "proj/game.das_project"));
    }

    TEST_CASE("Accepts a directory as the start path")
    {
        TempTree tmp;
        Touch(tmp / "proj/game.das_project");
        std::filesystem::create_directories(tmp / "proj/scripts");

        CHECK(duin::fs::FindProjectFile(tmp / "proj/scripts") == Norm(tmp / "proj/game.das_project"));
        CHECK(duin::fs::FindProjectFile(tmp / "proj/scripts/") == Norm(tmp / "proj/game.das_project"));
    }

    TEST_CASE("Works for a script that does not exist yet")
    {
        TempTree tmp;
        Touch(tmp / "proj/game.das_project");
        std::filesystem::create_directories(tmp / "proj/scripts");

        CHECK(duin::fs::FindProjectFile(tmp / "proj/scripts/new.das") == Norm(tmp / "proj/game.das_project"));
    }

    TEST_CASE("Nearest project file wins")
    {
        TempTree tmp;
        Touch(tmp / "proj/outer.das_project");
        Touch(tmp / "proj/scripts/inner.das_project");
        Touch(tmp / "proj/scripts/sub/main.das");

        CHECK(duin::fs::FindProjectFile(tmp / "proj/scripts/sub/main.das") ==
              Norm(tmp / "proj/scripts/inner.das_project"));
    }

    TEST_CASE("Picks the alphabetically first when a directory holds several")
    {
        TempTree tmp;
        Touch(tmp / "proj/zeta.das_project");
        Touch(tmp / "proj/alpha.das_project");
        Touch(tmp / "proj/mid.das_project");

        CHECK(duin::fs::FindProjectFile(tmp / "proj/main.das") == Norm(tmp / "proj/alpha.das_project"));
    }

    TEST_CASE("Finds a bare .das_project dotfile (DuinRT layout)")
    {
        TempTree tmp;
        Touch(tmp / "rt/.das_project");
        Touch(tmp / "rt/scripts/main.das");

        CHECK(duin::fs::FindProjectFile(tmp / "rt/scripts/main.das") == Norm(tmp / "rt/.das_project"));
    }

    TEST_CASE("Ignores look-alike names and directories")
    {
        TempTree tmp;
        Touch(tmp / "proj/real.das_project");
        Touch(tmp / "proj/sub/game.das_project.bak");
        Touch(tmp / "proj/sub/game.das_projectx");
        Touch(tmp / "proj/sub/das_project");
        Touch(tmp / "proj/sub/game.das");
        std::filesystem::create_directories(tmp / "proj/sub/folder.das_project");

        CHECK(duin::fs::FindProjectFile(tmp / "proj/sub/game.das") == Norm(tmp / "proj/real.das_project"));
    }

    TEST_CASE("Resolves virtual start paths (wrk://)")
    {
        TempTree tmp;
        Touch(tmp / "ws/game.das_project");
        std::filesystem::create_directories(tmp / "ws/scripts");
        duin::fs::SetWorkspacePath(tmp / "ws");

        CHECK(duin::fs::FindProjectFile("wrk://scripts/main.das") == Norm(tmp / "ws/game.das_project"));
    }

    TEST_CASE("Returns an absolute forward-slashed path for a relative start")
    {
        TempTree tmp("./artifacts");
        Touch(tmp / "proj/game.das_project");
        Touch(tmp / "proj/main.das");
        std::string relative =
            std::filesystem::relative(std::filesystem::u8path(tmp / "proj/main.das"), std::filesystem::current_path())
                .generic_string();

        std::string found = duin::fs::FindProjectFile(relative);
        CHECK(std::filesystem::path(found).is_absolute());
        CHECK(found.find('\\') == std::string::npos);
        CHECK(found == Norm(tmp / "proj/game.das_project"));
    }

    TEST_CASE("Returns INVALID_PATH when no project file exists up the tree")
    {
        TempTree tmp;
        Touch(tmp / "lonely/scripts/main.das");

        // Only meaningful if nothing above the temp dir carries a project file
        std::string found = duin::fs::FindProjectFile(tmp / "lonely/scripts/main.das");
        if (!duin::fs::IsPathInvalid(found))
        {
            MESSAGE("Skipped: a .das_project exists above the temp dir: " << found);
        }
        else
        {
            CHECK(found == INVALID_PATH);
        }
    }

    TEST_CASE("Returns INVALID_PATH for empty or invalid input")
    {
        CHECK(duin::fs::FindProjectFile("") == INVALID_PATH);
        CHECK(duin::fs::FindProjectFile(INVALID_PATH) == INVALID_PATH);
    }

    TEST_CASE("Resolves the real project file of every script-driven app")
    {
        // The apps call FindProjectFile(entryScript) instead of hard-coding the project path
        EngineRootGuard guard;
        std::string real = RealEngineRoot();
        REQUIRE_FALSE(duin::fs::IsPathInvalid(real));
        REQUIRE(duin::fs::SetEngineRoot(real));

        struct Case
        {
            const char *start;
            const char *project;
        };
        const Case cases[] = {
            {"eng://../DuinRT/src/main.cpp", "../DuinRT/.das_project"},
            {"eng://../DuinEditor/scripts/_entry.das", "../DuinEditor/duineditor.das_project"},
            {"eng://../ExampleProjects/DuinFPSDaslang/scripts/main.das",
             "../ExampleProjects/DuinFPSDaslang/duinfpsdaslang.das_project"},
            {"eng://../DaslangDocGenerator/scripts/docgen.das", "../DaslangDocGenerator/scripts/docgen.das_project"},
            {"eng://../DuinTests/scripts/Test_dn_uuid.das", "../DuinTests/duintests.das_project"},
            {"eng://src/Duin/Script/modules", "duin_engine.das_project"},
        };
        for (const Case &c : cases)
        {
            std::string expected = Norm(real + "/" + c.project);
            INFO("start: " << c.start);
            CHECK(duin::fs::FindProjectFile(c.start) == expected);
        }
    }
}

// =============================================================================
//  Regression guard: no hard-coded absolute paths in engine/app sources
// =============================================================================

TEST_SUITE("EnginePaths - No hard-coded absolute paths")
{
    TEST_CASE("Engine and app sources contain no drive-letter or MSYS absolute paths")
    {
        std::string engineRoot = RealEngineRoot();
        REQUIRE_FALSE(duin::fs::IsPathInvalid(engineRoot));
        const std::filesystem::path repo = std::filesystem::u8path(engineRoot).parent_path();

        // Mirrors tools/check_abs_paths.sh. Matches C:/x.., C:\x.., C:\\x.. and /c/Projects|Users|Program
        const std::regex absPath(
            R"((^|[^A-Za-z0-9_])[A-Za-z]:(\\\\|\\|/)[A-Za-z_][A-Za-z0-9_]|(^|["' =])/[a-z]/(Projects|Users|Program))");

        const char *scanDirs[] = {
            "Duin/src/Duin",     "DuinRT/src",          "DuinEditor/src",
            "DuinEditor/scripts", "DuinEditor/data",    "DuinDasHost/src",
            "DaslangDocGenerator/src", "DaslangDocGenerator/scripts", "ExampleProjects",
        };
        const std::vector<std::string> extensions = {".cpp", ".h", ".hpp", ".c", ".das", ".toml", ".json", ".lua"};
        const std::vector<std::string> skipDirs = {"bin", "bin-int", "vendor", "external", "modules"};
        // Documentation examples of path functions, not real locations
        const std::vector<std::string> skipFiles = {"Duin/src/Duin/IO/Filesystem.h"};

        // Generated caches legitimately record machine paths and are git-ignored:
        // dot-prefixed entries (.xxd_cache.json, .das_module.manifest, .jitted_scripts/, .cache/)
        // and anything named *cache*
        auto isGeneratedCache = [](std::string name) {
            std::transform(name.begin(), name.end(), name.begin(), [](unsigned char c) { return std::tolower(c); });
            return (!name.empty() && name[0] == '.') || name.find("cache") != std::string::npos;
        };

        std::vector<std::string> offenders;
        for (const char *dir : scanDirs)
        {
            std::filesystem::path base = repo / dir;
            if (!std::filesystem::exists(base))
            {
                continue;
            }
            for (auto it = std::filesystem::recursive_directory_iterator(base);
                 it != std::filesystem::recursive_directory_iterator(); ++it)
            {
                const std::string name = it->path().filename().generic_string();
                if (it->is_directory())
                {
                    if (std::find(skipDirs.begin(), skipDirs.end(), name) != skipDirs.end() || isGeneratedCache(name))
                    {
                        it.disable_recursion_pending();
                    }
                    continue;
                }
                if (isGeneratedCache(name))
                {
                    continue;
                }
                const std::string ext = it->path().extension().generic_string();
                if (std::find(extensions.begin(), extensions.end(), ext) == extensions.end())
                {
                    continue;
                }
                const std::string rel = std::filesystem::relative(it->path(), repo).generic_string();
                if (std::find(skipFiles.begin(), skipFiles.end(), rel) != skipFiles.end())
                {
                    continue;
                }

                std::ifstream file(it->path());
                std::string line;
                for (int lineNo = 1; std::getline(file, line); ++lineNo)
                {
                    if (std::regex_search(line, absPath))
                    {
                        offenders.push_back(rel + ":" + std::to_string(lineNo) + ": " + line);
                    }
                }
            }
        }

        for (const std::string &o : offenders)
        {
            MESSAGE("absolute path: " << o);
        }
        CHECK(offenders.empty());
    }
}

} // namespace TestEnginePaths
