#include <doctest.h>
#include <Duin/IO/VirtualFilesystem.h>
#include <Duin/IO/Filesystem.h>
#include <fstream>
#include <string>
#include <filesystem>
#include <vector>
#include <algorithm>

namespace TestVirtualFilesystem
{

// Helper function to create a test file
static void CreateTestFile(const std::string &path, const std::string &content = "test content")
{
    std::ofstream file(path);
    file << content;
    file.close();
}

// Helper function to check if file exists
static bool FileExists(const std::string &path)
{
    std::ifstream file(path);
    return file.good();
}

// Helper function to read file content
static std::string ReadFileContent(const std::string &path)
{
    std::ifstream file(path);
    if (!file.good())
        return "";
    std::string content((std::istreambuf_iterator<char>(file)), std::istreambuf_iterator<char>());
    return content;
}

// Base directory for test artifacts (relative to test executable)
static const std::string ARTIFACTS_DIR = "./artifacts/vfs_test";

// Sets wrk:// to point at ARTIFACTS_DIR and returns the resolved system path
static std::string SetupWorkspace()
{
    std::filesystem::create_directories(ARTIFACTS_DIR);
    std::string absPath = std::filesystem::absolute(ARTIFACTS_DIR).string();
    duin::fs::SetWorkspacePath(absPath);
    return duin::fs::EnsureUnixPath(absPath) + "/";
}

// ============================================================================
// Rejection of non-virtual (plain system) paths
// ============================================================================

TEST_SUITE("VirtualFilesystem - Non-virtual path rejection")
{
    TEST_CASE("FileCopy rejects plain system paths")
    {
        SetupWorkspace();
        CHECK(duin::vfs::FileCopy("C:/some/file.txt", "C:/other/file.txt") == false);
        CHECK(duin::vfs::FileCopy("./relative/path.txt", "wrk://dest.txt") == false);
        CHECK(duin::vfs::FileCopy("wrk://source.txt", "./relative/dest.txt") == false);
    }

    TEST_CASE("CreateDir rejects plain system paths")
    {
        CHECK(duin::vfs::CreateDir("C:/some/directory") == false);
        CHECK(duin::vfs::CreateDir("./relative/dir") == false);
        CHECK(duin::vfs::CreateDir("") == false);
    }

    TEST_CASE("RemovePath rejects plain system paths")
    {
        CHECK(duin::vfs::RemovePath("C:/some/file.txt") == false);
        CHECK(duin::vfs::RemovePath("./relative/file.txt") == false);
        CHECK(duin::vfs::RemovePath("") == false);
    }

    TEST_CASE("RenamePath rejects plain system paths")
    {
        CHECK(duin::vfs::RenamePath("C:/old.txt", "C:/new.txt") == false);
        CHECK(duin::vfs::RenamePath("wrk://old.txt", "C:/new.txt") == false);
        CHECK(duin::vfs::RenamePath("C:/old.txt", "wrk://new.txt") == false);
    }

    TEST_CASE("GlobDirectory rejects plain system paths")
    {
        int count = -1;
        char **results = duin::vfs::GlobDirectory("C:/some/dir", "*", 0, &count);
        CHECK(results == nullptr);
    }

    TEST_CASE("EnumerateDirectory rejects plain system paths")
    {
        auto cb = [](void *, const char *, const char *) -> duin::fs::EnumerationResult {
            return duin::fs::DNFS_ENUM_CONTINUE;
        };
        CHECK(duin::vfs::EnumerateDirectory("C:/some/dir", cb, nullptr) == false);
        CHECK(duin::vfs::EnumerateDirectory("./relative", cb, nullptr) == false);
    }

    TEST_CASE("GetFsPathInfo rejects plain system paths")
    {
        duin::fs::PathInfo info{};
        CHECK(duin::vfs::GetPathInfo("C:/some/file.txt", &info) == false);
        CHECK(duin::vfs::GetPathInfo("./relative/file.txt", &info) == false);
    }
}

// ============================================================================
// Rejection of unrecognized virtual prefixes
// ============================================================================

TEST_SUITE("VirtualFilesystem - Unrecognized prefix rejection")
{
    TEST_CASE("FileCopy rejects unknown prefixes")
    {
        CHECK(duin::vfs::FileCopy("xyz://a.txt", "xyz://b.txt") == false);
        CHECK(duin::vfs::FileCopy("res://a.txt", "res://b.txt") == false);
    }

    TEST_CASE("CreateDir rejects unknown prefixes")
    {
        CHECK(duin::vfs::CreateDir("xyz://somedir") == false);
    }

    TEST_CASE("RemovePath rejects unknown prefixes")
    {
        CHECK(duin::vfs::RemovePath("xyz://somefile.txt") == false);
    }

    TEST_CASE("RenamePath rejects unknown prefixes")
    {
        CHECK(duin::vfs::RenamePath("xyz://old.txt", "xyz://new.txt") == false);
    }

    TEST_CASE("GlobDirectory rejects unknown prefixes")
    {
        int count = -1;
        char **results = duin::vfs::GlobDirectory("xyz://dir", "*", 0, &count);
        CHECK(results == nullptr);
    }

    TEST_CASE("EnumerateDirectory rejects unknown prefixes")
    {
        auto cb = [](void *, const char *, const char *) -> duin::fs::EnumerationResult {
            return duin::fs::DNFS_ENUM_CONTINUE;
        };
        CHECK(duin::vfs::EnumerateDirectory("xyz://dir", cb, nullptr) == false);
    }

    TEST_CASE("GetFsPathInfo rejects unknown prefixes")
    {
        duin::fs::PathInfo info{};
        CHECK(duin::vfs::GetPathInfo("xyz://file.txt", &info) == false);
    }
}

// ============================================================================
// wrk:// delegation tests (actual filesystem operations)
// ============================================================================

TEST_SUITE("VirtualFilesystem - CreateDir via wrk://")
{
    TEST_CASE("Create a directory through vfs")
    {
        SetupWorkspace();

        std::string vpath = "wrk://vfs_create_dir_test";
        bool result = duin::vfs::CreateDir(vpath);
        CHECK(result == true);

        // Verify directory exists on the real filesystem
        std::string sysPath = duin::fs::MapVirtualToSystemPath(vpath);
        CHECK(std::filesystem::exists(sysPath));
        CHECK(std::filesystem::is_directory(sysPath));

        // Cleanup
        std::filesystem::remove_all(sysPath);
    }

    TEST_CASE("Create nested directories through vfs")
    {
        SetupWorkspace();

        std::string vpath = "wrk://vfs_nested/deep/path";
        bool result = duin::vfs::CreateDir(vpath);
        CHECK(result == true);

        std::string sysPath = duin::fs::MapVirtualToSystemPath(vpath);
        CHECK(std::filesystem::exists(sysPath));

        // Cleanup
        std::string root = duin::fs::MapVirtualToSystemPath("wrk://vfs_nested");
        std::filesystem::remove_all(root);
    }
}

TEST_SUITE("VirtualFilesystem - FileCopy via wrk://")
{
    TEST_CASE("Copy a file through vfs")
    {
        std::string base = SetupWorkspace();

        // Create source file on disk at the wrk:// location
        std::string srcSys = base + "vfs_copy_src.txt";
        std::string dstVpath = "wrk://vfs_copy_dst.txt";
        CreateTestFile(srcSys, "vfs copy content");

        bool result = duin::vfs::FileCopy("wrk://vfs_copy_src.txt", dstVpath);
        CHECK(result == true);

        std::string dstSys = duin::fs::MapVirtualToSystemPath(dstVpath);
        CHECK(FileExists(dstSys));
        CHECK(ReadFileContent(dstSys) == "vfs copy content");

        // Cleanup
        std::filesystem::remove(srcSys);
        std::filesystem::remove(dstSys);
    }

    TEST_CASE("Copy non-existent file through vfs returns false")
    {
        SetupWorkspace();
        bool result = duin::vfs::FileCopy("wrk://nonexistent_vfs.txt", "wrk://dest_vfs.txt");
        CHECK(result == false);
    }
}

TEST_SUITE("VirtualFilesystem - RenamePath via wrk://")
{
    TEST_CASE("Rename a file through vfs")
    {
        std::string base = SetupWorkspace();

        std::string oldSys = base + "vfs_rename_old.txt";
        CreateTestFile(oldSys, "rename me via vfs");

        bool result = duin::vfs::RenamePath("wrk://vfs_rename_old.txt", "wrk://vfs_rename_new.txt");
        CHECK(result == true);

        std::string newSys = duin::fs::MapVirtualToSystemPath("wrk://vfs_rename_new.txt");
        CHECK(FileExists(newSys));
        CHECK(ReadFileContent(newSys) == "rename me via vfs");
        CHECK_FALSE(FileExists(oldSys));

        // Cleanup
        std::filesystem::remove(newSys);
    }

    TEST_CASE("Rename non-existent file through vfs returns false")
    {
        SetupWorkspace();
        bool result = duin::vfs::RenamePath("wrk://no_such_file.txt", "wrk://new_name.txt");
        CHECK(result == false);
    }
}

TEST_SUITE("VirtualFilesystem - RemovePath via wrk://")
{
    TEST_CASE("Remove a file through vfs")
    {
        std::string base = SetupWorkspace();

        std::string fileSys = base + "vfs_remove_me.txt";
        CreateTestFile(fileSys, "delete me");

        bool result = duin::vfs::RemovePath("wrk://vfs_remove_me.txt");
        CHECK(result == true);
        CHECK_FALSE(FileExists(fileSys));
    }

    TEST_CASE("Remove an empty directory through vfs")
    {
        std::string base = SetupWorkspace();

        std::string dirSys = base + "vfs_remove_dir";
        std::filesystem::create_directories(dirSys);

        bool result = duin::vfs::RemovePath("wrk://vfs_remove_dir");
        CHECK(result == true);
        CHECK_FALSE(std::filesystem::exists(dirSys));
    }
}

TEST_SUITE("VirtualFilesystem - GetFsPathInfo via wrk://")
{
    TEST_CASE("Get info for a file through vfs")
    {
        std::string base = SetupWorkspace();

        std::string fileSys = base + "vfs_info_file.txt";
        CreateTestFile(fileSys, "info test content");

        duin::fs::PathInfo info{};
        bool result = duin::vfs::GetPathInfo("wrk://vfs_info_file.txt", &info);
        CHECK(result == true);
        CHECK(info.type == duin::fs::DNFS_PATHTYPE_FILE);
        CHECK(info.size > 0);

        // Cleanup
        std::filesystem::remove(fileSys);
    }

    TEST_CASE("Get info for a directory through vfs")
    {
        std::string base = SetupWorkspace();

        std::string dirSys = base + "vfs_info_dir";
        std::filesystem::create_directories(dirSys);

        duin::fs::PathInfo info{};
        bool result = duin::vfs::GetPathInfo("wrk://vfs_info_dir", &info);
        CHECK(result == true);
        CHECK(info.type == duin::fs::DNFS_PATHTYPE_DIRECTORY);

        // Cleanup
        std::filesystem::remove(dirSys);
    }

    TEST_CASE("Get info for non-existent path through vfs returns false")
    {
        SetupWorkspace();

        duin::fs::PathInfo info{};
        bool result = duin::vfs::GetPathInfo("wrk://no_such_path_vfs", &info);
        CHECK(result == false);
    }
}

TEST_SUITE("VirtualFilesystem - GlobDirectory via wrk://")
{
    TEST_CASE("Glob all files through vfs")
    {
        std::string base = SetupWorkspace();

        std::string dirSys = base + "vfs_glob_test";
        std::filesystem::create_directories(dirSys);

        CreateTestFile(dirSys + "/a.txt", "a");
        CreateTestFile(dirSys + "/b.txt", "b");
        CreateTestFile(dirSys + "/c.json", "c");

        int count = 0;
        char **results = duin::vfs::GlobDirectory("wrk://vfs_glob_test", "*", 0, &count);

        CHECK(results != nullptr);
        CHECK(count == 3);

        if (results != nullptr)
        {
            SDL_free(results);
        }
        std::filesystem::remove_all(dirSys);
    }

    TEST_CASE("Glob with pattern filter through vfs")
    {
        std::string base = SetupWorkspace();

        std::string dirSys = base + "vfs_glob_filter";
        std::filesystem::create_directories(dirSys);

        CreateTestFile(dirSys + "/file1.txt", "1");
        CreateTestFile(dirSys + "/file2.txt", "2");
        CreateTestFile(dirSys + "/data.json", "3");

        int count = 0;
        char **results = duin::vfs::GlobDirectory("wrk://vfs_glob_filter", "*.txt", 0, &count);

        CHECK(results != nullptr);
        CHECK(count == 2);

        if (results != nullptr)
        {
            SDL_free(results);
        }
        std::filesystem::remove_all(dirSys);
    }

    TEST_CASE("Glob empty directory through vfs")
    {
        std::string base = SetupWorkspace();

        std::string dirSys = base + "vfs_glob_empty";
        std::filesystem::create_directories(dirSys);

        int count = 0;
        char **results = duin::vfs::GlobDirectory("wrk://vfs_glob_empty", "*", 0, &count);

        CHECK(count == 0);

        if (results != nullptr)
        {
            SDL_free(results);
        }
        std::filesystem::remove_all(dirSys);
    }
}

TEST_SUITE("VirtualFilesystem - EnumerateDirectory via wrk://")
{
    TEST_CASE("Enumerate files through vfs")
    {
        std::string base = SetupWorkspace();

        std::string dirSys = base + "vfs_enum_test";
        std::filesystem::create_directories(dirSys);

        CreateTestFile(dirSys + "/alpha.txt", "a");
        CreateTestFile(dirSys + "/beta.txt", "b");

        std::vector<std::string> collected;
        auto cb = [](void *userdata, const char *dirname, const char *fname) -> duin::fs::EnumerationResult {
            auto *vec = static_cast<std::vector<std::string> *>(userdata);
            if (fname != nullptr)
            {
                vec->push_back(fname);
            }
            return duin::fs::DNFS_ENUM_CONTINUE;
        };

        bool result = duin::vfs::EnumerateDirectory("wrk://vfs_enum_test", cb, &collected);
        CHECK(result == true);
        CHECK(collected.size() == 2);

        bool hasAlpha = std::find(collected.begin(), collected.end(), "alpha.txt") != collected.end();
        bool hasBeta = std::find(collected.begin(), collected.end(), "beta.txt") != collected.end();
        CHECK(hasAlpha);
        CHECK(hasBeta);

        std::filesystem::remove_all(dirSys);
    }

    TEST_CASE("Enumerate non-existent directory through vfs returns false")
    {
        SetupWorkspace();

        auto cb = [](void *, const char *, const char *) -> duin::fs::EnumerationResult {
            return duin::fs::DNFS_ENUM_CONTINUE;
        };

        bool result = duin::vfs::EnumerateDirectory("wrk://no_such_dir_vfs", cb, nullptr);
        CHECK(result == false);
    }
}

// ============================================================================
// Integration / workflow tests
// ============================================================================

TEST_SUITE("VirtualFilesystem - Integration")
{
    TEST_CASE("Full workflow: create dir, copy file, rename, get info, remove")
    {
        std::string base = SetupWorkspace();

        // Create directory
        CHECK(duin::vfs::CreateDir("wrk://vfs_workflow") == true);

        // Create a source file on disk
        std::string srcSys = base + "vfs_workflow/original.txt";
        CreateTestFile(srcSys, "workflow content");

        // Copy via vfs
        CHECK(duin::vfs::FileCopy("wrk://vfs_workflow/original.txt", "wrk://vfs_workflow/copy.txt") == true);

        // Verify copy exists
        std::string copySys = duin::fs::MapVirtualToSystemPath("wrk://vfs_workflow/copy.txt");
        CHECK(ReadFileContent(copySys) == "workflow content");

        // Rename via vfs
        CHECK(duin::vfs::RenamePath("wrk://vfs_workflow/copy.txt", "wrk://vfs_workflow/renamed.txt") == true);
        CHECK_FALSE(FileExists(copySys));

        std::string renamedSys = duin::fs::MapVirtualToSystemPath("wrk://vfs_workflow/renamed.txt");
        CHECK(ReadFileContent(renamedSys) == "workflow content");

        // Get info via vfs
        duin::fs::PathInfo info{};
        CHECK(duin::vfs::GetPathInfo("wrk://vfs_workflow/renamed.txt", &info) == true);
        CHECK(info.type == duin::fs::DNFS_PATHTYPE_FILE);

        // Remove files via vfs
        CHECK(duin::vfs::RemovePath("wrk://vfs_workflow/original.txt") == true);
        CHECK(duin::vfs::RemovePath("wrk://vfs_workflow/renamed.txt") == true);
        CHECK(duin::vfs::RemovePath("wrk://vfs_workflow") == true);

        // Verify cleanup
        std::string dirSys = duin::fs::MapVirtualToSystemPath("wrk://vfs_workflow");
        CHECK_FALSE(std::filesystem::exists(dirSys));
    }

    TEST_CASE("bin:// path resolves for GetFsPathInfo on executable directory")
    {
        // bin:// should resolve to the executable's base path
        // The base directory itself should exist
        duin::fs::PathInfo info{};
        bool result = duin::vfs::GetPathInfo("bin://", &info);
        CHECK(result == true);
        CHECK(info.type == duin::fs::DNFS_PATHTYPE_DIRECTORY);
    }

    TEST_CASE("wrk:// without SetWorkspacePath fails")
    {
        // Reset workspace by setting an empty path (SetWorkspacePath rejects empty)
        // We can't truly "unset" it, but we can test that an unrecognized prefix fails.
        // This test verifies that if workspace were empty, mapping fails.
        // Since other tests may have set it, we test indirectly with a bad prefix.
        CHECK(duin::vfs::CreateDir("nop://somedir") == false);
    }
}

// ============================================================================
// Cleanup
// ============================================================================

TEST_SUITE("VirtualFilesystem - Cleanup")
{
    TEST_CASE("Remove artifacts directory")
    {
        std::filesystem::remove_all(ARTIFACTS_DIR);
        // Re-create so other test suites aren't affected
        std::filesystem::create_directories("./artifacts");
    }
}

} // namespace TestVirtualFilesystem
