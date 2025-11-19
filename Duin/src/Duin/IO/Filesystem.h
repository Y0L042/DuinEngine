#pragma once

#include <string>

/**
 * @namespace duin::fs
 * @brief Filesystem utilities for file and directory operations.
 *
 * This namespace provides cross-platform filesystem operations built on top of SDL3's
 * filesystem API. All paths use Unix-style forward slashes (/) as separators, regardless
 * of the underlying platform.
 *
 * Error Handling:
 * - Functions returning std::string return INVALID_PATH on error
 * - Functions returning bool return false on error, true on success
 * - Use IsPathInvalid() to check if a path string is valid
 *
 * @see https://wiki.libsdl.org/SDL3/CategoryFilesystem
 */
namespace duin::fs
{

/**
 * @brief Constant representing an invalid or error path state.
 *
 * This value is returned by filesystem functions when they encounter an error
 * or cannot produce a valid path. Use IsPathInvalid() to check if a path equals
 * this constant.
 */
#define INVALID_PATH "___INVALID_PATH___"

/**
 * @brief Extract the filename from a full path.
 *
 * Returns the portion of the path after the last forward slash (/).
 *
 * @param path The full path containing a filename (e.g., "path/to/file.txt")
 * @return The filename portion (e.g., "file.txt"), or INVALID_PATH if no separator is found
 *
 * @note Only Unix-style paths with forward slashes (/) are supported
 * @note Returns INVALID_PATH if the path contains no separator
 * @note Returns an empty string if the path ends with a separator
 *
 * Example:
 * @code
 * std::string filename = duin::fs::GetFileName("path/to/document.pdf");
 * // filename = "document.pdf"
 *
 * std::string invalid = duin::fs::GetFileName("file.txt"); // No separator
 * // invalid = INVALID_PATH
 * @endcode
 */
std::string GetFileName(const std::string &path);

/**
 * @brief Extract the file extension from a filename.
 *
 * Returns the portion of the filename after the last dot (.).
 * Does not include the dot itself in the returned extension.
 *
 * @param filename The filename to extract extension from (e.g., "file.txt")
 * @return The extension without the dot (e.g., "txt"), or INVALID_PATH if no extension exists
 *
 * @note Returns INVALID_PATH if no extension is found
 * @note Returns INVALID_PATH if the filename is just "." or starts with "."
 * @note Returns an empty string if the filename ends with a dot
 * @note For files with multiple dots (e.g., "archive.tar.gz"), returns only the last extension ("gz")
 *
 * Example:
 * @code
 * std::string ext1 = duin::fs::GetFileExtension("document.pdf");  // ext1 = "pdf"
 * std::string ext2 = duin::fs::GetFileExtension("archive.tar.gz"); // ext2 = "gz"
 * std::string ext3 = duin::fs::GetFileExtension("README");         // ext3 = INVALID_PATH
 * @endcode
 */
std::string GetFileExtension(const std::string &filename);

/**
 * @brief Copy a file from one location to another.
 *
 * Creates a copy of the file at oldPath and saves it to newPath.
 * If newPath already exists, it will be overwritten.
 *
 * @param oldPath Source file path (must exist)
 * @param newPath Destination file path (will be created or overwritten)
 * @return true if the copy succeeded, false otherwise
 *
 * @note Uses SDL_CopyFile internally
 * @note The source file must exist or the operation will fail
 * @note Parent directories of newPath must exist
 * @note File permissions and metadata may not be preserved
 *
 * @see https://wiki.libsdl.org/SDL3/SDL_CopyFile
 *
 * Example:
 * @code
 * bool success = duin::fs::FileCopy("source.txt", "backup.txt");
 * if (success) {
 *     // File copied successfully
 * }
 * @endcode
 */
bool FileCopy(const std::string &oldPath, const std::string &newPath);

/**
 * @brief Create a directory, including any missing parent directories.
 *
 * Creates the specified directory and all necessary parent directories in the path.
 * If the directory already exists, the operation succeeds without error.
 *
 * @param path Directory path to create (Unix-style with forward slashes)
 * @return true if the directory was created or already exists, false on error
 *
 * @note Uses SDL_CreateDirectory internally
 * @note Creates all parent directories automatically (like mkdir -p)
 * @note Returns true if the directory already exists
 * @note Returns false if path is empty or invalid
 *
 * @see https://wiki.libsdl.org/SDL3/SDL_CreateDirectory
 *
 * Example:
 * @code
 * bool success = duin::fs::CreateDir("path/to/nested/directory");
 * if (success) {
 *     // Directory and all parents created successfully
 * }
 * @endcode
 */
bool CreateDir(const std::string &path);

/**
 * @brief Get the directory where the application executable is located.
 *
 * Returns the absolute path to the directory containing the running application's
 * executable file. This is useful for locating resources bundled with the application.
 *
 * @return The base path with a trailing slash, or INVALID_PATH on error
 *
 * @note Uses SDL_GetBasePath internally
 * @note The returned path includes a trailing slash
 * @note This is the directory containing the executable, not the current working directory
 * @note On some platforms, this may be a sandbox or app bundle directory
 *
 * @see https://wiki.libsdl.org/SDL3/SDL_GetBasePath
 * @see GetCurrentDir()
 *
 * Example:
 * @code
 * std::string basePath = duin::fs::GetBasePath();
 * std::string configPath = basePath + "config.ini";
 * @endcode
 */
std::string GetBasePath(void);

/**
 * @brief Get the current working directory.
 *
 * Returns the absolute path of the current working directory.
 * This may differ from GetBasePath() if the application has changed directories
 * or was launched from a different location.
 *
 * @return The current working directory, or INVALID_PATH on error
 *
 * @note Uses SDL_GetCurrentDirectory internally
 * @note This is the directory that relative paths are resolved against
 * @note May change during application execution if directories are changed
 *
 * @see https://wiki.libsdl.org/SDL3/SDL_GetCurrentDirectory
 * @see GetBasePath()
 *
 * Example:
 * @code
 * std::string cwd = duin::fs::GetCurrentDir();
 * DN_INFO("Working directory: {}", cwd);
 * @endcode
 */
std::string GetCurrentDir(void);

/**
 * @brief Get the user's preference/settings directory for the application.
 *
 * Returns a platform-specific directory where the application should store user
 * preferences, settings, and save data. The directory is created if it doesn't exist.
 *
 * Platform-specific locations:
 * - Windows: %APPDATA%\org\app\
 * - macOS: ~/Library/Application Support/org/app/
 * - Linux: ~/.local/share/org/app/
 *
 * @param org Organization name (e.g., "MyCompany")
 * @param app Application name (e.g., "MyGame")
 * @return The preference path with a trailing slash, or INVALID_PATH on error
 *
 * @note Uses SDL_GetPrefPath internally
 * @note The directory is automatically created if it doesn't exist
 * @note The returned path includes a trailing slash
 * @note Returns INVALID_PATH if org or app are empty
 * @note The path is guaranteed to be writable by the user
 *
 * @see https://wiki.libsdl.org/SDL3/SDL_GetPrefPath
 * @see SetPrefPath()
 *
 * Example:
 * @code
 * std::string prefPath = duin::fs::GetPrefPath("MyCompany", "MyGame");
 * std::string savePath = prefPath + "savegame.dat";
 * @endcode
 */
std::string GetPrefPath(const std::string &org, const std::string &app);

/**
 * @brief Set global organization and application names for virtual path resolution.
 *
 * Stores organization and application names in global variables that are used by
 * MapVirtualToSystemPath() when resolving "app://" virtual paths. This allows you
 * to set these values once at application startup and use virtual paths throughout
 * your application without repeatedly specifying org/app names.
 *
 * @param org Organization name (e.g., "MyCompany")
 * @param app Application name (e.g., "MyGame")
 * @return Always returns true (currently cannot fail)
 *
 * @note This is a global setting that affects MapVirtualToSystemPath()
 * @note Should be called once during application initialization
 * @note Empty strings are allowed but may cause INVALID_PATH from MapVirtualToSystemPath()
 * @note Thread safety: Not thread-safe - set before multi-threaded operation begins
 *
 * @see GetPrefPath()
 * @see MapVirtualToSystemPath()
 *
 * Example:
 * @code
 * // At application startup
 * duin::fs::SetPrefPath("MyCompany", "MyGame");
 *
 * // Later, use virtual paths without specifying org/app again
 * std::string sysPath = duin::fs::MapVirtualToSystemPath("app://settings.json");
 * // sysPath = "/home/user/.local/share/MyCompany/MyGame/settings.json" (on Linux)
 * @endcode
 */
bool SetPrefPath(const std::string &org, const std::string &app);

/**
 * @brief Remove a file or directory.
 *
 * Removes the specified file or directory from the filesystem.
 * For directories, recursively removes all contents.
 *
 * @param path Path to file or directory to remove
 * @return true if removal succeeded, false otherwise
 *
 * @note Uses SDL_RemovePath internally
 * @note Recursively removes directory contents (like rm -rf)
 * @note Returns false if the path doesn't exist
 * @note Returns false if path is empty
 * @note Use with caution - removed files cannot be recovered
 *
 * @see https://wiki.libsdl.org/SDL3/SDL_RemovePath
 *
 * Example:
 * @code
 * bool success = duin::fs::RemovePath("temp/cache");
 * if (success) {
 *     // Directory and all contents removed
 * }
 * @endcode
 */
bool RemovePath(const std::string &path);

/**
 * @brief Rename or move a file or directory.
 *
 * Renames a file or directory from oldPath to newPath.
 * Can also be used to move files/directories to different locations.
 *
 * @param oldPath Current path (must exist)
 * @param newPath New path (will be created)
 * @return true if rename/move succeeded, false otherwise
 *
 * @note Uses SDL_RenamePath internally
 * @note Can rename files or directories
 * @note Can move items to different directories
 * @note If newPath exists, behavior is platform-dependent (may overwrite or fail)
 * @note Returns false if oldPath doesn't exist
 * @note Returns false if either path is empty
 * @note Atomic operation on most platforms
 *
 * @see https://wiki.libsdl.org/SDL3/SDL_RenamePath
 *
 * Example:
 * @code
 * bool success = duin::fs::RenamePath("old_name.txt", "new_name.txt");
 * if (success) {
 *     // File renamed successfully
 * }
 * @endcode
 */
bool RenamePath(const std::string &oldPath, const std::string &newPath);

/**
 * @brief Check if a path string represents an invalid/error state.
 *
 * Compares the given path against the INVALID_PATH constant to determine
 * if it represents an error condition.
 *
 * @param path The path string to check
 * @return true if the path equals INVALID_PATH, false otherwise
 *
 * @note An empty string is NOT considered invalid (returns false)
 * @note Only exact matches to INVALID_PATH return true
 *
 * Example:
 * @code
 * std::string path = duin::fs::GetFileName("no_separator");
 * if (duin::fs::IsPathInvalid(path)) {
 *     // Handle error - path extraction failed
 * }
 * @endcode
 */
bool IsPathInvalid(const std::string &path);

/**
 * @brief Convert Windows-style paths to Unix-style paths.
 *
 * Replaces all backslashes (\) with forward slashes (/) to normalize
 * path separators to Unix-style. This ensures consistent path handling
 * across all platforms.
 *
 * @param path The path to convert (may contain backslashes)
 * @return A new string with all backslashes replaced by forward slashes
 *
 * @note This function operates on a copy of the input string (pass-by-value)
 * @note If the path already uses Unix-style separators, it is returned unchanged
 * @note Mixed separators (both \ and /) are normalized to all forward slashes
 * @note This is automatically applied to paths returned by SDL3 filesystem functions
 * @note Works with Windows drive letters: "C:\\path" becomes "C:/path"
 * @note Works with UNC paths: "\\\\server\\share" becomes "//server/share"
 *
 * Example:
 * @code
 * std::string winPath = "C:\\Users\\Name\\Documents\\file.txt";
 * std::string unixPath = duin::fs::EnsureUnixPath(winPath);
 * // unixPath = "C:/Users/Name/Documents/file.txt"
 *
 * std::string mixed = "path\\to/mixed\\separators";
 * std::string normalized = duin::fs::EnsureUnixPath(mixed);
 * // normalized = "path/to/mixed/separators"
 *
 * std::string alreadyUnix = "path/to/file.txt";
 * std::string unchanged = duin::fs::EnsureUnixPath(alreadyUnix);
 * // unchanged = "path/to/file.txt"
 * @endcode
 */
std::string EnsureUnixPath(const std::string &path);

/**
 * @brief Map a virtual path to an absolute system path.
 *
 * Converts Godot-style virtual paths (e.g., "bin://", "app://") to absolute
 * system paths. This provides a convenient way to reference application directories
 * without hardcoding absolute paths.
 *
 * Supported virtual path prefixes:
 * - **bin://** - Maps to the application's base directory (executable location)
 * - **app://** - Maps to the application's user data directory (requires SetPrefPath)
 * - **usr://** - Maps to the user's folder (NOT YET IMPLEMENTED - returns INVALID_PATH)
 *
 * @param path Virtual path starting with a recognized prefix (e.g., "bin://config.ini")
 * @return Absolute system path, or INVALID_PATH if the prefix is unrecognized or path is too short
 *
 * @note Path must be at least 6 characters ("bin://x")
 * @note For "app://", you must call SetPrefPath() first to set org/app names
 * @note If SetPrefPath() wasn't called, "app://" paths will fail (GetPrefPath returns INVALID_PATH)
 * @note Returns INVALID_PATH for "usr://" (not yet implemented)
 * @note Returns INVALID_PATH if path is shorter than minimum length
 * @note The returned path uses Unix-style separators (forward slashes)
 * @note If the virtual prefix doesn't match any known prefix, returns INVALID_PATH
 *
 * @see SetPrefPath()
 * @see GetBasePath()
 * @see GetPrefPath()
 *
 * Example:
 * @code
 * // First, set preferences for app:// paths
 * duin::fs::SetPrefPath("MyCompany", "MyGame");
 *
 * // Map bin:// to executable directory
 * std::string configPath = duin::fs::MapVirtualToSystemPath("bin://config.ini");
 * // configPath = "/path/to/executable/config.ini"
 *
 * // Map app:// to user data directory
 * std::string savePath = duin::fs::MapVirtualToSystemPath("app://saves/game1.sav");
 * // savePath = "/home/user/.local/share/MyCompany/MyGame/saves/game1.sav" (Linux)
 *
 * // Invalid: path too short
 * std::string invalid1 = duin::fs::MapVirtualToSystemPath("bin:");
 * // invalid1 = INVALID_PATH
 *
 * // Invalid: unrecognized prefix
 * std::string invalid2 = duin::fs::MapVirtualToSystemPath("xyz://file.txt");
 * // invalid2 = INVALID_PATH
 *
 * // Not yet implemented
 * std::string notImpl = duin::fs::MapVirtualToSystemPath("usr://documents/file.txt");
 * // notImpl = INVALID_PATH
 * @endcode
 */
std::string MapVirtualToSystemPath(const std::string &path);

// TODO:
// EnumerateDirectory
// GetPathInfo
// GetUserFolder
// GlobDirectory

} // namespace duin::fs