#pragma once

// Shared helpers for engine-root / das-root / project-file resolution tests.
// Every helper is RAII so global state (env vars, cached engine root, temp dirs)
// is restored even when a CHECK/REQUIRE fails mid-test.

#include <Duin/IO/Filesystem.h>

#include <cstdlib>
#include <filesystem>
#include <fstream>
#include <optional>
#include <random>
#include <string>

namespace EnginePathsTest
{

// Absolute, forward-slashed, normalized, no trailing slash — the format the resolvers return
inline std::string Norm(const std::string &path)
{
    std::string out = std::filesystem::absolute(std::filesystem::u8path(path)).lexically_normal().generic_string();
    while (out.size() > 1 && out.back() == '/')
    {
        out.pop_back();
    }
    return out;
}

inline void Touch(const std::string &path)
{
    std::filesystem::create_directories(std::filesystem::u8path(path).parent_path());
    std::ofstream f(std::filesystem::u8path(path));
}

// Creates <base>/Duin/vendor/daslang/daslib/builtin.das; returns the engine root (<base>/Duin)
inline std::string MakeFakeEngineRoot(const std::string &base)
{
    std::string root = Norm(base + "/Duin");
    Touch(root + "/vendor/daslang/daslib/builtin.das");
    return root;
}

// Creates <dir>/daslib/builtin.das; returns the das root
inline std::string MakeFakeDasRoot(const std::string &dir)
{
    std::string root = Norm(dir);
    Touch(root + "/daslib/builtin.das");
    return root;
}

// Sets (or unsets, with std::nullopt) an environment variable for the scope's lifetime
class ScopedEnv
{
  public:
    ScopedEnv(const char *name, std::optional<std::string> value) : name(name)
    {
        if (const char *old = std::getenv(name))
        {
            previous = old;
        }
        Apply(value);
    }
    ~ScopedEnv()
    {
        Apply(previous);
    }
    ScopedEnv(const ScopedEnv &) = delete;
    ScopedEnv &operator=(const ScopedEnv &) = delete;

  private:
    std::string name;
    std::optional<std::string> previous;

    void Apply(const std::optional<std::string> &value)
    {
#ifdef _WIN32
        _putenv_s(name.c_str(), value ? value->c_str() : ""); // "" removes the variable
#else
        if (value)
            setenv(name.c_str(), value->c_str(), 1);
        else
            unsetenv(name.c_str());
#endif
    }
};

// Clears DUIN_ROOT and DUIN_DAS_ROOT so a developer's shell env can't skew results
struct ClearedPathEnv
{
    ScopedEnv root{"DUIN_ROOT", std::nullopt};
    ScopedEnv dasRoot{"DUIN_DAS_ROOT", std::nullopt};
};

// Restores the cached engine root (used by eng:// and ResolveDasRoot) on scope exit
class EngineRootGuard
{
  public:
    EngineRootGuard() : previous(duin::fs::GetEngineRoot())
    {
    }
    ~EngineRootGuard()
    {
        if (!duin::fs::IsPathInvalid(previous))
        {
            duin::fs::SetEngineRoot(previous);
        }
    }
    EngineRootGuard(const EngineRootGuard &) = delete;
    EngineRootGuard &operator=(const EngineRootGuard &) = delete;

  private:
    std::string previous;
};

// Unique directory, removed recursively on scope exit.
// Default location is the system temp dir: outside the repo, so walk-ups don't hit real
// project files. Pass a base (e.g. "./artifacts") when a test needs a CWD-relative tree.
class TempTree
{
  public:
    explicit TempTree(const std::string &base = std::filesystem::temp_directory_path().generic_string())
    {
        std::random_device rd;
        root = Norm(base + "/duin_paths_test_" + std::to_string(rd()) + std::to_string(rd()));
        std::filesystem::create_directories(std::filesystem::u8path(root));
    }
    ~TempTree()
    {
        std::error_code ec;
        std::filesystem::remove_all(std::filesystem::u8path(root), ec);
    }
    TempTree(const TempTree &) = delete;
    TempTree &operator=(const TempTree &) = delete;

    std::string operator/(const std::string &rel) const
    {
        return root + "/" + rel;
    }
    const std::string &Path() const
    {
        return root;
    }

  private:
    std::string root;
};

// Real engine root of the repo the tests run in (resolved with env overrides ignored)
inline std::string RealEngineRoot()
{
    ClearedPathEnv env;
    return duin::fs::ResolveEngineRoot();
}

} // namespace EnginePathsTest
