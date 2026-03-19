#include "dnpch.h"
#include "Filesystem.h"
#include "SDL3/SDL_filesystem.h"
#include <algorithm>

/**
 * @file Filesystem.cpp
 * @brief Implementation of cross-platform filesystem utilities.
 *
 * Wraps SDL3 filesystem functions to provide a consistent interface for
 * file and directory operations. All paths use Unix-style forward slashes.
 */

// Virtual path prefix size: "xxx://" = 6 characters, but we check for 5 to extract the prefix
static const int vpathSize = 6;
static const std::string sep = "/";

// Virtual path prefixes (Godot-style virtual filesystem)
const std::string APPROOT = "bin://"; // Application executable directory
const std::string APPDATA = "app://"; // Application user data directory
const std::string USRDATA = "usr://"; // User's home folder
const std::string WRKDATA = "wrk://"; // Workspace directory set by app/editor

// Global variables for app:// virtual path resolution
// Set via SetPrefPath(), used by MapVirtualToSystemPath()
static std::string ORG = "";
static std::string APP = "";

// Global variable for wrk:// virtual path resolution
// Set via SetWorkspacePath(), used by MapVirtualToSystemPath()
static std::string WORKSPACE_PATH = "";

// When true, bin:// resolves to the current working directory instead of the executable directory.
// Useful in debug/development to run from a source tree without copying assets next to the binary.
static bool BIN_USE_CURRENT_DIR = false;

std::string duin::fs::GetFileName(const std::string &path)
{
    char sep = '/'; // Unix-style separator only

    size_t i = path.rfind(sep, path.length());
    if (i != std::string::npos)
    {
        return (path.substr(i + 1, path.length() - i));
    }

    return (INVALID_PATH);
}

std::string duin::fs::GetFileExtension(const std::string &filename)
{
    if (filename.empty())
    {
        return INVALID_PATH;
    }
    if (filename == ".")
    {
        return INVALID_PATH;
    }

    const char *dot = strrchr(filename.c_str(), '.');
    if (!dot)
    {
        return INVALID_PATH;
    }

    size_t dotPos = dot - filename.c_str();
    if (dotPos == 0)
    {
        std::string ext = dot + 1;
        if (ext.empty())
        {
            return INVALID_PATH;
        }
        return ext;
    }
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
    out = EnsureUnixPath(out);
    return out;
}

void duin::fs::SetBinDebugMode(bool enabled)
{
    BIN_USE_CURRENT_DIR = enabled;
}

bool duin::fs::GetBinDebugMode()
{
    return BIN_USE_CURRENT_DIR;
}

std::string duin::fs::GetBinPath()
{
    return BIN_USE_CURRENT_DIR ? GetCurrentDir() : GetBasePath();
}

std::string duin::fs::GetCurrentDir(void)
{
    // Get the current working directory from SDL3
    // Returns NULL on failure
    const char *res = SDL_GetCurrentDirectory();
    std::string out = (res != NULL) ? res : INVALID_PATH;
    out = EnsureUnixPath(out);
    return out;
}

std::string duin::fs::GetPrefPath(const std::string &org, const std::string &app)
{
    // Get platform-specific user preference directory from SDL3
    // Creates the directory if it doesn't exist
    // Returns NULL on failure (e.g., empty org/app names)
    const char *res = SDL_GetPrefPath(org.c_str(), app.c_str());
    std::string out = (res != NULL) ? res : INVALID_PATH;
    out = EnsureUnixPath(out);
    return out;
}

bool duin::fs::SetPrefPath(const std::string &org, const std::string &app)
{
    // Store organization and application names in global variables
    // These are used by MapVirtualToSystemPath() when resolving "app://" paths
    ORG = org;
    APP = app;

    return true;
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

std::string duin::fs::EnsureUnixPath(const std::string &path)
{
    // Create a copy of the input path (function uses pass-by-value)
    // Replace all backslashes with forward slashes to normalize to Unix-style
    std::string normalized = path;
    std::replace(normalized.begin(), normalized.end(), '\\', '/');
    return normalized;
}

std::string duin::fs::MapVirtualToSystemPath(const std::string &path)
{
    // Virtual paths must be at least "xxx://" (6 chars minimum)
    // Paths received from SDL contains trailing '/'
    if (path.size() < vpathSize - 1)
    {
        return INVALID_PATH;
    }

    std::string drive = path.substr(0, vpathSize);
    std::string sysPath = INVALID_PATH;

    if (drive == APPROOT)
    {
        sysPath = GetBinPath() + path.substr(vpathSize, path.size());
    }

    if (drive == APPDATA)
    {
        // Only map if ORG and APP have been set to non-empty values
        if (ORG.empty() || APP.empty())
        {
            sysPath = INVALID_PATH;
        }
        else
        {
            sysPath = GetPrefPath(ORG, APP) + path.substr(vpathSize, path.size());
        }
    }

    if (drive == USRDATA)
    {
        std::string home = GetUserFolder(DNFS_FOLDER_HOME);
        if (IsPathInvalid(home))
        {
            sysPath = INVALID_PATH;
        }
        else
        {
            sysPath = home + path.substr(vpathSize, path.size());
        }
    }

    if (drive == WRKDATA)
    {
        if (WORKSPACE_PATH.empty())
        {
            sysPath = INVALID_PATH;
        }
        else
        {
            sysPath = WORKSPACE_PATH + path.substr(vpathSize, path.size());
        }
    }

    return sysPath;
}

std::string duin::fs::MapSystemToVirtualPath(const std::string &path)
{
    if (path.empty())
    {
        return INVALID_PATH;
    }

    std::string normalized = EnsureUnixPath(path);

    // Check wrk:// first (most specific, user-set workspace)
    if (!WORKSPACE_PATH.empty() && normalized.rfind(WORKSPACE_PATH, 0) == 0)
    {
        return WRKDATA + normalized.substr(WORKSPACE_PATH.size());
    }

    // Check app:// (user data directory)
    if (!ORG.empty() && !APP.empty())
    {
        std::string prefPath = GetPrefPath(ORG, APP);
        if (!IsPathInvalid(prefPath) && normalized.rfind(prefPath, 0) == 0)
        {
            return APPDATA + normalized.substr(prefPath.size());
        }
    }

    // Check usr:// (user home directory)
    std::string home = GetUserFolder(DNFS_FOLDER_HOME);
    if (!IsPathInvalid(home) && normalized.rfind(home, 0) == 0)
    {
        return USRDATA + normalized.substr(home.size());
    }

    // Check bin:// (executable directory or cwd in debug mode)
    std::string basePath = GetBinPath();
    if (!IsPathInvalid(basePath) && normalized.rfind(basePath, 0) == 0)
    {
        return APPROOT + normalized.substr(basePath.size());
    }

    return INVALID_PATH;
}

bool duin::fs::IsVirtualPath(const std::string &path)
{
    if (path.size() < vpathSize)
    {
        return false;
    }

    std::string drive = path.substr(0, vpathSize);
    return drive == APPROOT || drive == APPDATA || drive == USRDATA || drive == WRKDATA;
}

char **duin::fs::GlobDirectory(const std::string &path, const std::string &pattern, GlobFlags flags, int *count)
{
    return ::SDL_GlobDirectory(path.c_str(), pattern.c_str(), flags, count);
}

SDL_EnumerationResult duin::fs::AdapterDirCallback(void *userdata, const char *dirname, const char *fname)
{
    auto *wrapper = static_cast<DirCallbackWrapper *>(userdata);
    return static_cast<SDL_EnumerationResult>(wrapper->callback(wrapper->userdata, dirname, fname));
}

bool duin::fs::EnumerateDirectory(const char *path, EnumerateDirectoryCallback callback, void *userdata)
{
    DirCallbackWrapper wrapper{callback, userdata};
    return ::SDL_EnumerateDirectory(path, AdapterDirCallback, &wrapper);
}

bool duin::fs::GetPathInfo(const std::string &path, PathInfo *info)
{
    if (!info)
    {
        return false;
    }

    SDL_PathInfo sdlInfo;
    if (!SDL_GetPathInfo(path.c_str(), &sdlInfo))
    {
        return false;
    }

    info->type = static_cast<PathType>(sdlInfo.type);
    info->size = sdlInfo.size;
    info->createTime = sdlInfo.create_time;
    info->modifyTime = sdlInfo.modify_time;
    info->accessTime = sdlInfo.access_time;
    return true;
}

std::string duin::fs::GetUserFolder(UserFolder folder)
{
    const char *res = SDL_GetUserFolder(static_cast<SDL_Folder>(folder));
    std::string out = (res != NULL) ? res : INVALID_PATH;
    out = EnsureUnixPath(out);
    return out;
}

bool duin::fs::SetWorkspacePath(const std::string &path)
{
    if (path.empty())
    {
        return false;
    }

    WORKSPACE_PATH = EnsureUnixPath(path);
    if (WORKSPACE_PATH.back() != '/')
    {
        WORKSPACE_PATH += '/';
    }

    return true;
}

std::string duin::fs::GetWorkspacePath()
{
    if (WORKSPACE_PATH.empty())
    {
        return INVALID_PATH;
    }
    return WORKSPACE_PATH;
}
