#pragma once

#include <string>
#include "Filesystem.h"

namespace duin::vfs
{

bool FileCopy(const std::string &oldPath, const std::string &newPath);
bool CreateDir(const std::string &path);
bool RemovePath(const std::string &path);
bool RenamePath(const std::string &oldPath, const std::string &newPath);
char **GlobDirectory(const std::string &path, const std::string &pattern, fs::GlobFlags flags, int *count);
bool EnumerateDirectory(const std::string &path, fs::EnumerateDirectoryCallback callback, void *userdata);
bool GetPathInfo(const std::string &path, fs::PathInfo *info = nullptr);




bool IsPathInvalid(const std::string &path); // TODO NUKE THIS!!!

} // namespace duin::vfs
