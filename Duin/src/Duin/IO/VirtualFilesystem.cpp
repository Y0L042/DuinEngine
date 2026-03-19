#include "dnpch.h"
#include "VirtualFilesystem.h"

namespace duin::vfs
{
// TODO duplication! Filesystem.cpp contains Truth
const std::string APPROOT_ = "bin://"; // Application executable directory
const std::string APPDATA_ = "app://"; // Application user data directory
const std::string USRDATA_ = "usr://"; // User's home folder
const std::string WRKDATA_ = "wrk://"; // Workspace directory set by app/editor
} // namespace duin::vfs

namespace duin::vfs
{
const std::string APPROOT()
{
    return APPROOT_;
}
const std::string APPDATA()
{
    return APPDATA_;
}
const std::string USRDATA()
{
    return USRDATA_;
}
const std::string WRKDATA()
{
    return WRKDATA_;
}

bool FileCopy(const std::string &oldPath, const std::string &newPath)
{
    std::string resolvedOld = fs::MapVirtualToSystemPath(oldPath);
    if (fs::IsPathInvalid(resolvedOld))
        return false;

    std::string resolvedNew = fs::MapVirtualToSystemPath(newPath);
    if (fs::IsPathInvalid(resolvedNew))
        return false;

    return fs::FileCopy(resolvedOld, resolvedNew);
}

bool CreateDir(const std::string &path)
{
    std::string resolved = fs::MapVirtualToSystemPath(path);
    if (fs::IsPathInvalid(resolved))
        return false;

    return fs::CreateDir(resolved);
}

bool RemovePath(const std::string &path)
{
    std::string resolved = fs::MapVirtualToSystemPath(path);
    if (fs::IsPathInvalid(resolved))
        return false;

    return fs::RemovePath(resolved);
}

bool RenamePath(const std::string &oldPath, const std::string &newPath)
{
    std::string resolvedOld = fs::MapVirtualToSystemPath(oldPath);
    if (fs::IsPathInvalid(resolvedOld))
        return false;

    std::string resolvedNew = fs::MapVirtualToSystemPath(newPath);
    if (fs::IsPathInvalid(resolvedNew))
        return false;

    return fs::RenamePath(resolvedOld, resolvedNew);
}

char **GlobDirectory(const std::string &path, const std::string &pattern, fs::GlobFlags flags, int *count)
{
    std::string resolved = fs::MapVirtualToSystemPath(path);
    if (fs::IsPathInvalid(resolved))
        return NULL;

    return fs::GlobDirectory(resolved, pattern, flags, count);
}

bool EnumerateDirectory(const std::string &path, fs::EnumerateDirectoryCallback callback, void *userdata)
{
    std::string resolved = fs::MapVirtualToSystemPath(path);
    if (fs::IsPathInvalid(resolved))
        return false;

    return fs::EnumerateDirectory(resolved.c_str(), callback, userdata);
}

bool GetPathInfo(const std::string &path, fs::PathInfo *info)
{
    std::string resolved = fs::MapVirtualToSystemPath(path);
    if (fs::IsPathInvalid(resolved))
        return false;

    return fs::GetPathInfo(resolved, info);
}

std::string OverridePathPrefix(const std::string &path, const std::string &prefix)
{
    static constexpr size_t vpathSize = 6; // "xxx://" = 6 characters

    if (path.size() < vpathSize || !fs::IsVirtualPath(path))
    {
        DN_WARN("OverridePathPrefix: path '{}' is not a valid virtual path!", path);
        return std::string();
    }

    if (prefix.size() > vpathSize)
    {
        DN_WARN("OverridePathPrefix: prefix '{}' exceeds maximum length of {} characters!", prefix, vpathSize);
    }

    if (!fs::IsVirtualPath(prefix + path.substr(vpathSize)))
    {
        DN_WARN("OverridePathPrefix: prefix '{}' is not a recognized virtual prefix!", prefix);
        return std::string();
    }

    return prefix + path.substr(vpathSize);
}

bool IsPathInvalid(const std::string &path)
{
    std::string resolved = fs::MapVirtualToSystemPath(path);
    return fs::IsPathInvalid(resolved);
}

} // namespace duin::vfs
