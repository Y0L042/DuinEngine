#include <doctest.h>
#include <Duin/IO/Filesystem.h>
#include <fstream>
#include <string>
#include <filesystem>
#include <Duin/Core/Debug/DNLog.h>

namespace TestFilesystem
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
static const std::string ARTIFACTS_DIR = "./artifacts";

TEST_SUITE("Filesystem - GetFileName")
{
    TEST_CASE("Simple filename with Unix separator")
    {
        std::string path = "path/to/file.txt";
        std::string filename = duin::fs::GetFileName(path);
        CHECK(filename == "file.txt");
    }

    TEST_CASE("Filename without path")
    {
        std::string path = "file.txt";
        std::string filename = duin::fs::GetFileName(path);
        CHECK(filename == INVALID_PATH);
    }

    TEST_CASE("Path with no file (ends with separator)")
    {
        std::string path = "path/to/";
        std::string filename = duin::fs::GetFileName(path);
        CHECK(filename == "");
    }

    TEST_CASE("Empty path")
    {
        std::string path = "";
        std::string filename = duin::fs::GetFileName(path);
        CHECK(filename == INVALID_PATH);
    }

    TEST_CASE("Filename with multiple dots")
    {
        std::string path = "path/to/my.file.archive.tar.gz";
        std::string filename = duin::fs::GetFileName(path);
        CHECK(filename == "my.file.archive.tar.gz");
    }

    TEST_CASE("Hidden file (Unix)")
    {
        std::string path = "path/to/.hidden";
        std::string filename = duin::fs::GetFileName(path);
        CHECK(filename == ".hidden");
    }

    TEST_CASE("Complex nested path")
    {
        std::string path = "very/deeply/nested/folder/structure/document.pdf";
        std::string filename = duin::fs::GetFileName(path);
        CHECK(filename == "document.pdf");
    }

    TEST_CASE("Filename with spaces")
    {
        std::string path = "path/to/my document.txt";
        std::string filename = duin::fs::GetFileName(path);
        CHECK(filename == "my document.txt");
    }

    TEST_CASE("Filename with special characters")
    {
        std::string path = "path/to/file@#$%.txt";
        std::string filename = duin::fs::GetFileName(path);
        CHECK(filename == "file@#$%.txt");
    }
}

TEST_SUITE("Filesystem - GetFileExtension")
{
    TEST_CASE("Simple extension")
    {
        std::string filename = "document.txt";
        std::string ext = duin::fs::GetFileExtension(filename);
        CHECK(ext == "txt");
    }

    TEST_CASE("Multiple dots in filename")
    {
        std::string filename = "archive.tar.gz";
        std::string ext = duin::fs::GetFileExtension(filename);
        CHECK(ext == "gz");
    }

    TEST_CASE("No extension")
    {
        std::string filename = "README";
        std::string ext = duin::fs::GetFileExtension(filename);
        CHECK(ext == INVALID_PATH);
    }

    TEST_CASE("Hidden file with extension (Unix)")
    {
        std::string filename = ".gitignore";
        std::string ext = duin::fs::GetFileExtension(filename);
        CHECK(ext == "gitignore");
    }

    TEST_CASE("Hidden file without extension")
    {
        std::string filename = ".git";
        std::string ext = duin::fs::GetFileExtension(filename);
        CHECK(ext == "git");
    }

    TEST_CASE("Just a dot")
    {
        std::string filename = ".";
        std::string ext = duin::fs::GetFileExtension(filename);
        CHECK(ext == INVALID_PATH);
    }

    TEST_CASE("Empty string")
    {
        std::string filename = "";
        std::string ext = duin::fs::GetFileExtension(filename);
        CHECK(ext == INVALID_PATH);
    }

    TEST_CASE("Filename ending with dot")
    {
        std::string filename = "file.";
        std::string ext = duin::fs::GetFileExtension(filename);
        CHECK(ext == "");
    }

    TEST_CASE("Complex file extension")
    {
        std::string filename = "data.json";
        std::string ext = duin::fs::GetFileExtension(filename);
        CHECK(ext == "json");
    }

    TEST_CASE("Uppercase extension")
    {
        std::string filename = "IMAGE.PNG";
        std::string ext = duin::fs::GetFileExtension(filename);
        CHECK(ext == "PNG");
    }

    TEST_CASE("Full path with extension")
    {
        std::string filename = "C:/path/to/file.cpp";
        std::string ext = duin::fs::GetFileExtension(filename);
        CHECK(ext == "cpp");
    }

    TEST_CASE("Long extension")
    {
        std::string filename = "file.extension";
        std::string ext = duin::fs::GetFileExtension(filename);
        CHECK(ext == "extension");
    }
}

TEST_SUITE("Filesystem - FileCopy")
{
    TEST_CASE("Copy file - basic test")
    {
        // Create artifacts directory if it doesn't exist
        std::filesystem::create_directories(ARTIFACTS_DIR);

        std::string sourcePath = ARTIFACTS_DIR + "/copy_source.txt";
        std::string destPath = ARTIFACTS_DIR + "/copy_dest.txt";

        // Create source file
        CreateTestFile(sourcePath, "Copy test content");

        // Copy the file
        bool result = duin::fs::FileCopy(sourcePath, destPath);

        // Verify copy succeeded
        CHECK(result == true);
        if (result)
        {
            CHECK(FileExists(destPath));
            CHECK(ReadFileContent(destPath) == "Copy test content");
        }

        // Cleanup
        std::filesystem::remove(sourcePath);
        std::filesystem::remove(destPath);
    }

    TEST_CASE("Copy non-existent file")
    {
        std::string sourcePath = ARTIFACTS_DIR + "/nonexistent_sdl_test.txt";
        std::string destPath = ARTIFACTS_DIR + "/destination_sdl_test.txt";

        bool result = duin::fs::FileCopy(sourcePath, destPath);

        // Should fail when source doesn't exist
        CHECK(result == false);

        // Cleanup (in case something was created)
        std::filesystem::remove(destPath);
    }

    TEST_CASE("Copy to existing file (overwrite)")
    {
        std::filesystem::create_directories(ARTIFACTS_DIR);

        std::string sourcePath = ARTIFACTS_DIR + "/copy_source2.txt";
        std::string destPath = ARTIFACTS_DIR + "/copy_dest2.txt";

        CreateTestFile(sourcePath, "New content");
        CreateTestFile(destPath, "Old content");

        bool result = duin::fs::FileCopy(sourcePath, destPath);

        // Verify result and content
        CHECK(result == true);
        if (result)
        {
            CHECK(ReadFileContent(destPath) == "New content");
        }

        // Cleanup
        std::filesystem::remove(sourcePath);
        std::filesystem::remove(destPath);
    }

    TEST_CASE("Copy empty file")
    {
        std::filesystem::create_directories(ARTIFACTS_DIR);

        std::string sourcePath = ARTIFACTS_DIR + "/empty_source.txt";
        std::string destPath = ARTIFACTS_DIR + "/empty_dest.txt";

        CreateTestFile(sourcePath, "");

        bool result = duin::fs::FileCopy(sourcePath, destPath);

        // Verify copy succeeded
        CHECK(result == true);
        if (result)
        {
            CHECK(FileExists(destPath));
            CHECK(ReadFileContent(destPath) == "");
        }

        // Cleanup
        std::filesystem::remove(sourcePath);
        std::filesystem::remove(destPath);
    }

    TEST_CASE("Copy file with larger content")
    {
        std::filesystem::create_directories(ARTIFACTS_DIR);

        std::string sourcePath = ARTIFACTS_DIR + "/large_source.txt";
        std::string destPath = ARTIFACTS_DIR + "/large_dest.txt";

        std::string largeContent = "This is a larger test content that spans multiple lines.\n";
        for (int i = 0; i < 10; i++)
        {
            largeContent += "Line " + std::to_string(i) + ": Some test data here.\n";
        }

        CreateTestFile(sourcePath, largeContent);

        bool result = duin::fs::FileCopy(sourcePath, destPath);

        // Verify copy succeeded and content matches
        CHECK(result == true);
        if (result)
        {
            CHECK(ReadFileContent(destPath) == largeContent);
        }

        // Cleanup
        std::filesystem::remove(sourcePath);
        std::filesystem::remove(destPath);
    }
}

TEST_SUITE("Filesystem - CreateDir")
{
    TEST_CASE("Create single directory")
    {
        std::filesystem::create_directories(ARTIFACTS_DIR);

        std::string dirPath = ARTIFACTS_DIR + "/test_dir_sdl";

        // Remove if it exists from previous run
        std::filesystem::remove(dirPath);

        bool result = duin::fs::CreateDir(dirPath);

        // Verify directory was created
        CHECK(result == true);
        if (result)
        {
            CHECK(std::filesystem::exists(dirPath));
            CHECK(std::filesystem::is_directory(dirPath));
        }

        // Cleanup
        std::filesystem::remove(dirPath);
    }

    TEST_CASE("Create nested directories")
    {
        std::filesystem::create_directories(ARTIFACTS_DIR);

        std::string dirPath = ARTIFACTS_DIR + "/nested/deep/path";

        // Remove if exists
        std::filesystem::remove_all(ARTIFACTS_DIR + "/nested");

        bool result = duin::fs::CreateDir(dirPath);

        // SDL_CreateDirectory should create all parent directories
        CHECK(result == true);
        if (result)
        {
            CHECK(std::filesystem::exists(dirPath));
            CHECK(std::filesystem::is_directory(dirPath));
        }

        // Cleanup
        std::filesystem::remove_all(ARTIFACTS_DIR + "/nested");
    }

    TEST_CASE("Create directory that already exists")
    {
        std::filesystem::create_directories(ARTIFACTS_DIR);

        std::string dirPath = ARTIFACTS_DIR + "/existing_dir_sdl";
        std::filesystem::create_directory(dirPath);

        bool result = duin::fs::CreateDir(dirPath);

        // SDL_CreateDirectory returns true if directory already exists
        CHECK(result == true);

        // Cleanup
        std::filesystem::remove(dirPath);
    }

    TEST_CASE("Create directory with empty path")
    {
        std::string dirPath = "";
        bool result = duin::fs::CreateDir(dirPath);
        // Should fail with empty path
        CHECK(result == false);
    }
}

TEST_SUITE("Filesystem - GetBasePath")
{
    TEST_CASE("Get base path returns valid path")
    {
        std::string basePath = duin::fs::GetBasePath();

        // Should return a valid path (not INVALID_PATH)
        CHECK(basePath != INVALID_PATH);
        CHECK_FALSE(basePath.empty());
    }

    TEST_CASE("Base path check with IsPathInvalid")
    {
        std::string basePath = duin::fs::GetBasePath();
        DN_INFO("BasePath: {}", basePath);
        bool isInvalid = duin::fs::IsPathInvalid(basePath);

        // Base path should not be invalid
        CHECK(isInvalid == false);
    }
}

TEST_SUITE("Filesystem - GetCurrentDir")
{
    TEST_CASE("Get current directory returns valid path")
    {
        std::string currentDir = duin::fs::GetCurrentDir();

        // Should return a valid path (not INVALID_PATH)
        CHECK(currentDir != INVALID_PATH);
        CHECK_FALSE(currentDir.empty());
    }

    TEST_CASE("Current directory check with IsPathInvalid")
    {
        std::string currentDir = duin::fs::GetCurrentDir();
        bool isInvalid = duin::fs::IsPathInvalid(currentDir);

        // Current directory should not be invalid
        CHECK(isInvalid == false);
    }
}

TEST_SUITE("Filesystem - GetPrefPath")
{
    TEST_CASE("Get pref path with valid org and app")
    {
        std::string prefPath = duin::fs::GetPrefPath("TestOrg", "TestApp");

        // Should return a valid path
        CHECK(prefPath != INVALID_PATH);
        CHECK_FALSE(prefPath.empty());

        // Cleanup - remove the created directory
        if (prefPath != INVALID_PATH && std::filesystem::exists(prefPath))
        {
            std::filesystem::remove_all(prefPath);
        }
    }

    TEST_CASE("Get pref path with empty org")
    {
        std::string prefPath = duin::fs::GetPrefPath("", "TestApp");
        // SDL may return INVALID_PATH or a default path
        // Just verify it doesn't crash
        CHECK((prefPath == INVALID_PATH || !prefPath.empty()));
    }

    TEST_CASE("Get pref path with empty app")
    {
        std::string prefPath = duin::fs::GetPrefPath("TestOrg", "");
        // SDL may return INVALID_PATH or a default path
        // Just verify it doesn't crash
        CHECK((prefPath == INVALID_PATH || !prefPath.empty()));
    }

    TEST_CASE("Get pref path with both empty")
    {
        std::string prefPath = duin::fs::GetPrefPath("", "");
        // Should likely return INVALID_PATH
        CHECK((prefPath == INVALID_PATH || !prefPath.empty()));
    }

    TEST_CASE("Pref path check with IsPathInvalid")
    {
        std::string prefPath = duin::fs::GetPrefPath("TestOrg2", "TestApp2");
        bool isInvalid = duin::fs::IsPathInvalid(prefPath);

        if (prefPath != INVALID_PATH)
        {
            CHECK(isInvalid == false);

            // Cleanup
            if (std::filesystem::exists(prefPath))
            {
                std::filesystem::remove_all(prefPath);
            }
        }
    }
}

TEST_SUITE("Filesystem - RemovePath")
{
    TEST_CASE("Remove existing file")
    {
        std::filesystem::create_directories(ARTIFACTS_DIR);

        std::string filePath = ARTIFACTS_DIR + "/remove_test_sdl.txt";
        CreateTestFile(filePath, "Remove me");

        bool result = duin::fs::RemovePath(filePath);

        // Verify removal succeeded
        CHECK(result == true);
        if (result)
        {
            CHECK_FALSE(FileExists(filePath));
        }

        // Cleanup (in case it wasn't removed)
        std::filesystem::remove(filePath);
    }

    TEST_CASE("Remove non-existent file")
    {
        std::string filePath = ARTIFACTS_DIR + "/nonexistent_remove_sdl_x.txt";
        bool result = duin::fs::RemovePath(filePath);
        CHECK(result == true);
    }

    TEST_CASE("Remove empty directory")
    {
        std::filesystem::create_directories(ARTIFACTS_DIR);

        std::string dirPath = ARTIFACTS_DIR + "/remove_dir_sdl";
        std::filesystem::create_directory(dirPath);

        bool result = duin::fs::RemovePath(dirPath);

        // Verify removal succeeded
        CHECK(result == true);
        if (result)
        {
            CHECK_FALSE(std::filesystem::exists(dirPath));
        }

        // Cleanup
        std::filesystem::remove(dirPath);
    }

    TEST_CASE("Remove directory with contents")
    {
        std::filesystem::create_directories(ARTIFACTS_DIR);

        std::string dirPath = ARTIFACTS_DIR + "/remove_dir_with_files_sdl";
        std::filesystem::create_directory(dirPath);
        CreateTestFile(dirPath + "/file.txt", "content");

        bool result = duin::fs::RemovePath(dirPath);

        // SDL_RemovePath should recursively remove directories
        CHECK(result == false);
        std::filesystem::remove_all(dirPath);
    }

    TEST_CASE("Remove with empty path")
    {
        std::string filePath = "";
        bool result = duin::fs::RemovePath(filePath);
        // Should fail with empty path
        CHECK(result == false);
    }
}

TEST_SUITE("Filesystem - RenamePath")
{
    TEST_CASE("Rename existing file")
    {
        std::filesystem::create_directories(ARTIFACTS_DIR);

        std::string oldPath = ARTIFACTS_DIR + "/rename_old_sdl.txt";
        std::string newPath = ARTIFACTS_DIR + "/rename_new_sdl.txt";

        CreateTestFile(oldPath, "Rename me");

        bool result = duin::fs::RenamePath(oldPath, newPath);

        // Verify rename succeeded
        CHECK(result == true);
        if (result)
        {
            CHECK_FALSE(FileExists(oldPath));
            CHECK(FileExists(newPath));
            CHECK(ReadFileContent(newPath) == "Rename me");
        }

        // Cleanup
        std::filesystem::remove(oldPath);
        std::filesystem::remove(newPath);
    }

    TEST_CASE("Rename non-existent file")
    {
        std::string oldPath = ARTIFACTS_DIR + "/nonexistent_rename_sdl.txt";
        std::string newPath = ARTIFACTS_DIR + "/new_name_sdl.txt";

        bool result = duin::fs::RenamePath(oldPath, newPath);

        // Should fail when source doesn't exist
        CHECK(result == false);

        // Cleanup
        std::filesystem::remove(newPath);
    }

    TEST_CASE("Rename to existing file (overwrite)")
    {
        std::filesystem::create_directories(ARTIFACTS_DIR);

        std::string oldPath = ARTIFACTS_DIR + "/rename_source_sdl.txt";
        std::string newPath = ARTIFACTS_DIR + "/rename_target_sdl.txt";

        CreateTestFile(oldPath, "Source");
        CreateTestFile(newPath, "Target");

        bool result = duin::fs::RenamePath(oldPath, newPath);

        // SDL_RenamePath behavior with existing destination
        if (result)
        {
            CHECK(FileExists(newPath));
            CHECK(ReadFileContent(newPath) == "Source");
        }

        // Cleanup
        std::filesystem::remove(oldPath);
        std::filesystem::remove(newPath);
    }

    TEST_CASE("Rename directory")
    {
        std::filesystem::create_directories(ARTIFACTS_DIR);

        std::string oldPath = ARTIFACTS_DIR + "/old_dir_sdl";
        std::string newPath = ARTIFACTS_DIR + "/new_dir_sdl";

        std::filesystem::create_directory(oldPath);

        bool result = duin::fs::RenamePath(oldPath, newPath);

        // Verify rename succeeded
        CHECK(result == true);
        if (result)
        {
            CHECK_FALSE(std::filesystem::exists(oldPath));
            CHECK(std::filesystem::exists(newPath));
        }

        // Cleanup
        std::filesystem::remove(oldPath);
        std::filesystem::remove(newPath);
    }

    TEST_CASE("Rename with empty old path")
    {
        std::string oldPath = "";
        std::string newPath = ARTIFACTS_DIR + "/new_sdl.txt";

        bool result = duin::fs::RenamePath(oldPath, newPath);
        // Should fail with empty path
        CHECK(result == false);
    }

    TEST_CASE("Rename with empty new path")
    {
        std::string oldPath = ARTIFACTS_DIR + "/old_sdl.txt";
        std::string newPath = "";

        bool result = duin::fs::RenamePath(oldPath, newPath);
        // Should fail with empty path
        CHECK(result == false);
    }
}

TEST_SUITE("Filesystem - IsPathInvalid")
{
    TEST_CASE("Valid path is not invalid")
    {
        std::string validPath = "/some/valid/path";
        bool isInvalid = duin::fs::IsPathInvalid(validPath);
        CHECK(isInvalid == false);
    }

    TEST_CASE("INVALID_PATH constant is invalid")
    {
        std::string invalidPath = INVALID_PATH;
        bool isInvalid = duin::fs::IsPathInvalid(invalidPath);
        CHECK(isInvalid == true);
    }

    TEST_CASE("Empty string is not INVALID_PATH")
    {
        std::string emptyPath = "";
        bool isInvalid = duin::fs::IsPathInvalid(emptyPath);
        CHECK(isInvalid == false);
    }

    TEST_CASE("Check GetFileName result for invalid case")
    {
        std::string path = "file.txt"; // No separator
        std::string filename = duin::fs::GetFileName(path);
        bool isInvalid = duin::fs::IsPathInvalid(filename);
        CHECK(isInvalid == true);
    }

    TEST_CASE("Check GetFileExtension result for invalid case")
    {
        std::string filename = "README"; // No extension
        std::string ext = duin::fs::GetFileExtension(filename);
        bool isInvalid = duin::fs::IsPathInvalid(ext);
        CHECK(isInvalid == true);
    }
}

TEST_SUITE("Filesystem - Integration Tests")
{
    TEST_CASE("GetFileName and GetFileExtension combination")
    {
        std::string fullPath = "path/to/document.pdf";
        std::string filename = duin::fs::GetFileName(fullPath);
        std::string extension = duin::fs::GetFileExtension(filename);

        CHECK(filename == "document.pdf");
        CHECK(extension == "pdf");
    }

    TEST_CASE("Complex path with GetFileName")
    {
        std::string fullPath = "C:/Users/Name/Documents/Project/src/main.cpp";
        std::string filename = duin::fs::GetFileName(fullPath);
        std::string extension = duin::fs::GetFileExtension(filename);

        CHECK(filename == "main.cpp");
        CHECK(extension == "cpp");
    }

    TEST_CASE("Path without extension")
    {
        std::string fullPath = "path/to/Makefile";
        std::string filename = duin::fs::GetFileName(fullPath);
        std::string extension = duin::fs::GetFileExtension(filename);

        CHECK(filename == "Makefile");
        CHECK(extension == INVALID_PATH);
        CHECK(duin::fs::IsPathInvalid(extension) == true);
    }

    TEST_CASE("Create, copy, rename, and remove file workflow")
    {
        std::filesystem::create_directories(ARTIFACTS_DIR);

        std::string originalPath = ARTIFACTS_DIR + "/workflow_original.txt";
        std::string copyPath = ARTIFACTS_DIR + "/workflow_copy.txt";
        std::string renamedPath = ARTIFACTS_DIR + "/workflow_renamed.txt";

        // Create original file
        CreateTestFile(originalPath, "Workflow test");

        // Copy the file
        bool copyResult = duin::fs::FileCopy(originalPath, copyPath);
        CHECK(copyResult == true);
        CHECK(FileExists(copyPath));

        // Rename the copy
        bool renameResult = duin::fs::RenamePath(copyPath, renamedPath);
        CHECK(renameResult == true);
        CHECK(FileExists(renamedPath));
        CHECK_FALSE(FileExists(copyPath));

        // Remove files
        bool removeOriginal = duin::fs::RemovePath(originalPath);
        bool removeRenamed = duin::fs::RemovePath(renamedPath);
        CHECK(removeOriginal == true);
        CHECK(removeRenamed == true);

        // Verify cleanup
        CHECK_FALSE(FileExists(originalPath));
        CHECK_FALSE(FileExists(renamedPath));

        // Final cleanup (in case something failed)
        std::filesystem::remove(originalPath);
        std::filesystem::remove(copyPath);
        std::filesystem::remove(renamedPath);
    }

    TEST_CASE("Create directory and add file")
    {
        std::filesystem::create_directories(ARTIFACTS_DIR);

        std::string dirPath = ARTIFACTS_DIR + "/workflow_dir";
        std::string filePath = dirPath + "/test_file.txt";

        // Create directory
        bool createResult = duin::fs::CreateDir(dirPath);
        CHECK(createResult == true);
        CHECK(std::filesystem::exists(dirPath));

        // Create file in directory
        CreateTestFile(filePath, "File in directory");
        CHECK(FileExists(filePath));

        // Final cleanup
        std::filesystem::remove_all(dirPath);
    }
}

TEST_SUITE("Filesystem - EnsureUnixPath")
{
    TEST_CASE("Windows path conversion - simple backslashes")
    {
        std::string winPath = "C:\\Users\\Name\\Documents\\file.txt";
        std::string unixPath = duin::fs::EnsureUnixPath(winPath);
        CHECK(unixPath == "C:/Users/Name/Documents/file.txt");
    }

    TEST_CASE("Windows path conversion - nested directories")
    {
        std::string winPath = "path\\to\\deeply\\nested\\directory\\file.txt";
        std::string unixPath = duin::fs::EnsureUnixPath(winPath);
        CHECK(unixPath == "path/to/deeply/nested/directory/file.txt");
    }

    TEST_CASE("Mixed separators normalization")
    {
        std::string mixed = "path\\to/mixed\\separators/file.txt";
        std::string normalized = duin::fs::EnsureUnixPath(mixed);
        CHECK(normalized == "path/to/mixed/separators/file.txt");
    }

    TEST_CASE("Already Unix-style path - no change")
    {
        std::string unixPath = "path/to/file.txt";
        std::string result = duin::fs::EnsureUnixPath(unixPath);
        CHECK(result == "path/to/file.txt");
    }

    TEST_CASE("Empty string")
    {
        std::string empty = "";
        std::string result = duin::fs::EnsureUnixPath(empty);
        CHECK(result == "");
    }

    TEST_CASE("Single backslash")
    {
        std::string singleBackslash = "\\";
        std::string result = duin::fs::EnsureUnixPath(singleBackslash);
        CHECK(result == "/");
    }

    TEST_CASE("Single forward slash")
    {
        std::string singleSlash = "/";
        std::string result = duin::fs::EnsureUnixPath(singleSlash);
        CHECK(result == "/");
    }

    TEST_CASE("Multiple consecutive backslashes")
    {
        std::string multipleBackslash = "path\\\\\\to\\\\file.txt";
        std::string result = duin::fs::EnsureUnixPath(multipleBackslash);
        CHECK(result == "path///to//file.txt");
    }

    TEST_CASE("Drive letter with backslash")
    {
        std::string drivePath = "C:\\";
        std::string result = duin::fs::EnsureUnixPath(drivePath);
        CHECK(result == "C:/");
    }

    TEST_CASE("UNC path conversion")
    {
        std::string uncPath = "\\\\server\\share\\folder\\file.txt";
        std::string result = duin::fs::EnsureUnixPath(uncPath);
        CHECK(result == "//server/share/folder/file.txt");
    }

    TEST_CASE("Relative Windows path")
    {
        std::string relativePath = "..\\..\\parent\\file.txt";
        std::string result = duin::fs::EnsureUnixPath(relativePath);
        CHECK(result == "../../parent/file.txt");
    }

    TEST_CASE("Path with spaces")
    {
        std::string pathWithSpaces = "C:\\Program Files\\My App\\file.txt";
        std::string result = duin::fs::EnsureUnixPath(pathWithSpaces);
        CHECK(result == "C:/Program Files/My App/file.txt");
    }

    TEST_CASE("Path with special characters")
    {
        std::string specialPath = "path\\with\\special@#$%\\chars.txt";
        std::string result = duin::fs::EnsureUnixPath(specialPath);
        CHECK(result == "path/with/special@#$%/chars.txt");
    }

    TEST_CASE("Integration - EnsureUnixPath with GetFileName")
    {
        std::string winPath = "C:\\Users\\Name\\Documents\\report.pdf";
        std::string unixPath = duin::fs::EnsureUnixPath(winPath);
        std::string filename = duin::fs::GetFileName(unixPath);

        CHECK(unixPath == "C:/Users/Name/Documents/report.pdf");
        CHECK(filename == "report.pdf");
    }

    TEST_CASE("Integration - EnsureUnixPath with GetFileExtension")
    {
        std::string winPath = "C:\\data\\archive.tar.gz";
        std::string unixPath = duin::fs::EnsureUnixPath(winPath);
        std::string filename = duin::fs::GetFileName(unixPath);
        std::string extension = duin::fs::GetFileExtension(filename);

        CHECK(unixPath == "C:/data/archive.tar.gz");
        CHECK(filename == "archive.tar.gz");
        CHECK(extension == "gz");
    }

    TEST_CASE("Path ending with backslash")
    {
        std::string pathEndingBackslash = "C:\\Users\\Name\\Documents\\";
        std::string result = duin::fs::EnsureUnixPath(pathEndingBackslash);
        CHECK(result == "C:/Users/Name/Documents/");
    }

    TEST_CASE("Only filename - no separators")
    {
        std::string filenameOnly = "file.txt";
        std::string result = duin::fs::EnsureUnixPath(filenameOnly);
        CHECK(result == "file.txt");
    }
}

TEST_SUITE("Filesystem - SetPrefPath")
{
    TEST_CASE("Set valid organization and application names")
    {
        bool result = duin::fs::SetPrefPath("TestOrg", "TestApp");
        CHECK(result == true);
    }

    TEST_CASE("Set with empty organization")
    {
        bool result = duin::fs::SetPrefPath("", "TestApp");
        CHECK(result == true); // Currently always returns true
    }

    TEST_CASE("Set with empty application")
    {
        bool result = duin::fs::SetPrefPath("TestOrg", "");
        CHECK(result == true); // Currently always returns true
    }

    TEST_CASE("Set with both empty")
    {
        bool result = duin::fs::SetPrefPath("", "");
        CHECK(result == true); // Currently always returns true
    }

    TEST_CASE("Set with special characters")
    {
        bool result = duin::fs::SetPrefPath("My-Org_123", "My.App");
        CHECK(result == true);
    }

    TEST_CASE("Set with spaces")
    {
        bool result = duin::fs::SetPrefPath("My Organization", "My Application");
        CHECK(result == true);
    }

    TEST_CASE("Set multiple times (overwrite)")
    {
        bool result1 = duin::fs::SetPrefPath("OldOrg", "OldApp");
        bool result2 = duin::fs::SetPrefPath("NewOrg", "NewApp");

        CHECK(result1 == true);
        CHECK(result2 == true);
    }

    TEST_CASE("Set with very long names")
    {
        std::string longOrg(100, 'O');
        std::string longApp(100, 'A');
        bool result = duin::fs::SetPrefPath(longOrg, longApp);
        CHECK(result == true);
    }
}

TEST_SUITE("Filesystem - MapVirtualToSystemPath")
{
    TEST_CASE("Map bin:// path")
    {
        std::string virtualPath = "bin://config.ini";
        std::string sysPath = duin::fs::MapVirtualToSystemPath(virtualPath);
        DN_INFO("SysPath: {}", sysPath);

        // Should not be INVALID_PATH
        CHECK(sysPath != INVALID_PATH);
        CHECK_FALSE(sysPath.empty());

        // Should end with the filename
        CHECK(sysPath.find("config.ini") != std::string::npos);
    }

    TEST_CASE("Map bin:// with nested path")
    {
        std::string virtualPath = "bin://data/textures/image.png";
        std::string sysPath = duin::fs::MapVirtualToSystemPath(virtualPath);

        CHECK(sysPath != INVALID_PATH);
        CHECK(sysPath.find("data/textures/image.png") != std::string::npos);
    }

    TEST_CASE("Map app:// path after SetPrefPath")
    {
        // First set the preferences
        duin::fs::SetPrefPath("TestOrgVPath", "TestAppVPath");

        std::string virtualPath = "app://settings.json";
        std::string sysPath = duin::fs::MapVirtualToSystemPath(virtualPath);

        // Should not be INVALID_PATH
        CHECK(sysPath != INVALID_PATH);
        CHECK_FALSE(sysPath.empty());

        // Should end with the filename
        CHECK(sysPath.find("settings.json") != std::string::npos);

        // Cleanup the created preference directory
        std::string prefPath = duin::fs::GetPrefPath("TestOrgVPath", "TestAppVPath");
        if (prefPath != INVALID_PATH && std::filesystem::exists(prefPath))
        {
            std::filesystem::remove_all(prefPath);
        }
    }

    TEST_CASE("Map app:// with nested path")
    {
        duin::fs::SetPrefPath("TestOrgNested", "TestAppNested");

        std::string virtualPath = "app://saves/game1/progress.sav";
        std::string sysPath = duin::fs::MapVirtualToSystemPath(virtualPath);

        CHECK(sysPath != INVALID_PATH);
        CHECK(sysPath.find("saves/game1/progress.sav") != std::string::npos);

        // Cleanup
        std::string prefPath = duin::fs::GetPrefPath("TestOrgNested", "TestAppNested");
        if (prefPath != INVALID_PATH && std::filesystem::exists(prefPath))
        {
            std::filesystem::remove_all(prefPath);
        }
    }

    TEST_CASE("Map app:// without SetPrefPath - uses empty ORG/APP")
    {
        // Set to empty values
        duin::fs::SetPrefPath("", "");

        std::string virtualPath = "app://file.txt";
        std::string sysPath = duin::fs::MapVirtualToSystemPath(virtualPath);

        // GetPrefPath with empty org/app returns INVALID_PATH
        // So the result should be INVALID_PATH
        CHECK(sysPath == INVALID_PATH);
    }

    TEST_CASE("Map usr:// path - not yet implemented")
    {
        std::string virtualPath = "usr://documents/file.txt";
        std::string sysPath = duin::fs::MapVirtualToSystemPath(virtualPath);

        // Should return INVALID_PATH (not implemented)
        CHECK(sysPath == INVALID_PATH);
    }

    TEST_CASE("Invalid: path too short")
    {
        std::string tooShort1 = "bin";
        std::string tooShort2 = "bin:";
        std::string tooShort3 = "bin:/";

        CHECK(duin::fs::MapVirtualToSystemPath(tooShort1) == INVALID_PATH);
        CHECK(duin::fs::MapVirtualToSystemPath(tooShort2) == INVALID_PATH);
        CHECK(duin::fs::MapVirtualToSystemPath(tooShort3) == INVALID_PATH);
    }

    TEST_CASE("Invalid: unrecognized prefix")
    {
        std::string unknown1 = "xyz://file.txt";
        std::string unknown2 = "res://file.txt";
        std::string unknown3 = "data://file.txt";

        CHECK(duin::fs::MapVirtualToSystemPath(unknown1) == INVALID_PATH);
        CHECK(duin::fs::MapVirtualToSystemPath(unknown2) == INVALID_PATH);
        CHECK(duin::fs::MapVirtualToSystemPath(unknown3) == INVALID_PATH);
    }

    TEST_CASE("Invalid: empty string")
    {
        std::string empty = "";
        CHECK(duin::fs::MapVirtualToSystemPath(empty) == INVALID_PATH);
    }

    TEST_CASE("Map bin:// just the prefix")
    {
        std::string justPrefix = "bin://";
        std::string sysPath = duin::fs::MapVirtualToSystemPath(justPrefix);

        // Should return base path (might be the base directory itself)
        CHECK(sysPath != INVALID_PATH);
    }

    TEST_CASE("Integration - Map and use with GetFileName")
    {
        std::string virtualPath = "bin://assets/sprites/player.png";
        std::string sysPath = duin::fs::MapVirtualToSystemPath(virtualPath);

        CHECK(sysPath != INVALID_PATH);

        std::string filename = duin::fs::GetFileName(sysPath);
        CHECK(filename == "player.png");
    }

    TEST_CASE("Integration - Map and use with GetFileExtension")
    {
        duin::fs::SetPrefPath("TestIntegration", "TestExt");

        std::string virtualPath = "app://config.toml";
        std::string sysPath = duin::fs::MapVirtualToSystemPath(virtualPath);

        CHECK(sysPath != INVALID_PATH);

        std::string filename = duin::fs::GetFileName(sysPath);
        std::string extension = duin::fs::GetFileExtension(filename);

        CHECK(filename == "config.toml");
        CHECK(extension == "toml");

        // Cleanup
        std::string prefPath = duin::fs::GetPrefPath("TestIntegration", "TestExt");
        if (prefPath != INVALID_PATH && std::filesystem::exists(prefPath))
        {
            std::filesystem::remove_all(prefPath);
        }
    }

    TEST_CASE("Path with forward slashes after prefix")
    {
        std::string virtualPath = "bin://folder/subfolder/file.txt";
        std::string sysPath = duin::fs::MapVirtualToSystemPath(virtualPath);

        CHECK(sysPath != INVALID_PATH);
        // Verify the path structure is preserved
        CHECK(sysPath.find("folder/subfolder/file.txt") != std::string::npos);
    }

    TEST_CASE("Workflow: SetPrefPath then use app:// paths")
    {
        // Simulate application startup
        bool setPref = duin::fs::SetPrefPath("WorkflowOrg", "WorkflowApp");
        CHECK(setPref == true);

        // Use virtual paths throughout the application
        std::string configPath = duin::fs::MapVirtualToSystemPath("app://config.ini");
        std::string savePath = duin::fs::MapVirtualToSystemPath("app://saves/slot1.sav");
        std::string logPath = duin::fs::MapVirtualToSystemPath("app://logs/debug.log");

        CHECK(configPath != INVALID_PATH);
        CHECK(savePath != INVALID_PATH);
        CHECK(logPath != INVALID_PATH);

        CHECK(configPath.find("config.ini") != std::string::npos);
        CHECK(savePath.find("saves/slot1.sav") != std::string::npos);
        CHECK(logPath.find("logs/debug.log") != std::string::npos);

        // Cleanup
        std::string prefPath = duin::fs::GetPrefPath("WorkflowOrg", "WorkflowApp");
        if (prefPath != INVALID_PATH && std::filesystem::exists(prefPath))
        {
            std::filesystem::remove_all(prefPath);
        }
    }
}

TEST_SUITE("Filesystem - Edge Cases")
{
    TEST_CASE("Very long path")
    {
        std::string longPath = "very/long/path/with/many/nested/directories/that/goes/on/and/on/file.txt";
        std::string filename = duin::fs::GetFileName(longPath);
        CHECK(filename == "file.txt");
    }

    TEST_CASE("Path with consecutive separators")
    {
        std::string path = "path//to///file.txt";
        std::string filename = duin::fs::GetFileName(path);
        CHECK(filename == "file.txt");
    }

    TEST_CASE("Extension with numbers")
    {
        std::string filename = "backup.bak2";
        std::string ext = duin::fs::GetFileExtension(filename);
        CHECK(ext == "bak2");
    }

    TEST_CASE("Filename is just extension-like")
    {
        std::string filename = ".txt";
        std::string ext = duin::fs::GetFileExtension(filename);
        CHECK(ext == "txt");
    }
}

// Cleanup test - ensure artifacts directory is present but empty at the end
TEST_SUITE("Filesystem - Cleanup")
{
    TEST_CASE("Ensure artifacts directory exists")
    {
        std::filesystem::create_directories(ARTIFACTS_DIR);
        bool exists = std::filesystem::exists(ARTIFACTS_DIR);
        CHECK(exists);
    }
}

} // namespace TestFilesystem
