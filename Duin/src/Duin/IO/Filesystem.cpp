#include "dnpch.h"
#include "Filesystem.h"
#include "SDL3/SDL_filesystem.h"

/**
 * @file Filesystem.cpp
 * @brief Implementation of cross-platform filesystem utilities.
 *
 * Wraps SDL3 filesystem functions to provide a consistent interface for
 * file and directory operations. All paths use Unix-style forward slashes.
 */

std::string duin::fs::GetFileName(const std::string &path)
{
    char sep = '/'; // Unix-style separator only

    size_t i = path.rfind(sep, path.length());
    if (i != std::string::npos)
    {
        // Return substring after the last separator
        return (path.substr(i + 1, path.length() - i));
    }

    // No separator found - invalid path
    return (INVALID_PATH);
}

std::string duin::fs::GetFileExtension(const std::string &filename)
{
    // Find the last dot in the filename
    const char *dot = strrchr(filename.c_str(), '.');

    // No dot found or dot is at the beginning (hidden file like ".git")
    if (!dot || dot == filename)
        return INVALID_PATH;

    // Return everything after the dot (excluding the dot itself)
    return dot + 1;
}

bool duin::fs::FileCopy(const std::string &oldPath, const std::string &newPath)
{
    // Delegate to SDL3's file copy function
    // Returns true on success, false on failure
    bool res = SDL_CopyFile(oldPath.c_str(), newPath.c_str());
    return res;
}

bool duin::fs::CreateDir(const std::string &path)
{
    // Delegate to SDL3's directory creation function
    // Automatically creates parent directories if needed (like mkdir -p)
    bool res = SDL_CreateDirectory(path.c_str());
    return res;
}

std::string duin::fs::GetBasePath(void)
{
    // Get the application's base directory from SDL3
    // Returns NULL on failure
    const char *res = SDL_GetBasePath();
    std::string out = (res != NULL) ? res : INVALID_PATH;
    return out;
}

std::string duin::fs::GetCurrentDir(void)
{
    // Get the current working directory from SDL3
    // Returns NULL on failure
    const char *res = SDL_GetCurrentDirectory();
    std::string out = (res != NULL) ? res : INVALID_PATH;
    return out;
}

std::string duin::fs::GetPrefPath(const std::string &org, const std::string &app)
{
    // Get platform-specific user preference directory from SDL3
    // Creates the directory if it doesn't exist
    // Returns NULL on failure (e.g., empty org/app names)
    const char *res = SDL_GetPrefPath(org.c_str(), app.c_str());
    std::string out = (res != NULL) ? res : INVALID_PATH;
    return out;
}

bool duin::fs::RemovePath(const std::string &path)
{
    // Delegate to SDL3's path removal function
    // Recursively removes directories and their contents
    bool res = SDL_RemovePath(path.c_str());
    return res;
}

bool duin::fs::RenamePath(const std::string &oldPath, const std::string &newPath)
{
    // Delegate to SDL3's path rename/move function
    // Can be used to rename or move files/directories
    bool res = SDL_RenamePath(oldPath.c_str(), newPath.c_str());
    return res;
}

bool duin::fs::IsPathInvalid(const std::string &path)
{
    // Compare against INVALID_PATH constant
    // Returns true (non-zero) if paths match, false (zero) if different
    bool res = !path.compare(INVALID_PATH);
    return res;
}
