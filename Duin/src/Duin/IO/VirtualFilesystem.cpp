#include "dnpch.h"
#include "VirtualFilesystem.h"

namespace duin::vfs
{

bool FileCopy(const std::string &oldPath, const std::string &newPath)
{
    std::string resolvedOld = fs::MapVirtualToSystemPath(oldPath);
    if (fs::IsPathInvalid(resolvedOld)) return false;

    std::string resolvedNew = fs::MapVirtualToSystemPath(newPath);
    if (fs::IsPathInvalid(resolvedNew)) return false;

    return fs::FileCopy(resolvedOld, resolvedNew);
}

bool CreateDir(const std::string &path)
{
    std::string resolved = fs::MapVirtualToSystemPath(path);
    if (fs::IsPathInvalid(resolved)) return false;

    return fs::CreateDir(resolved);
}

bool RemovePath(const std::string &path)
{
    std::string resolved = fs::MapVirtualToSystemPath(path);
    if (fs::IsPathInvalid(resolved)) return false;

    return fs::RemovePath(resolved);
}

bool RenamePath(const std::string &oldPath, const std::string &newPath)
{
    std::string resolvedOld = fs::MapVirtualToSystemPath(oldPath);
    if (fs::IsPathInvalid(resolvedOld)) return false;

    std::string resolvedNew = fs::MapVirtualToSystemPath(newPath);
    if (fs::IsPathInvalid(resolvedNew)) return false;

    return fs::RenamePath(resolvedOld, resolvedNew);
}

char **GlobDirectory(const std::string &path, const std::string &pattern, fs::GlobFlags flags, int *count)
{
    std::string resolved = fs::MapVirtualToSystemPath(path);
    if (fs::IsPathInvalid(resolved)) return NULL;

    return fs::GlobDirectory(resolved, pattern, flags, count);
}

bool EnumerateDirectory(const std::string &path, fs::EnumerateDirectoryCallback callback, void *userdata)
{
    std::string resolved = fs::MapVirtualToSystemPath(path);
    if (fs::IsPathInvalid(resolved)) return false;

    return fs::EnumerateDirectory(resolved.c_str(), callback, userdata);
}

bool GetPathInfo(const std::string &path, fs::PathInfo *info)
{
    std::string resolved = fs::MapVirtualToSystemPath(path);
    if (fs::IsPathInvalid(resolved)) return false;

    return fs::GetPathInfo(resolved, info);
}

bool IsPathInvalid(const std::string &path)
{
    std::string resolved = fs::MapVirtualToSystemPath(path);
    return fs::IsPathInvalid(resolved);
}

} // namespace duin::vfs
