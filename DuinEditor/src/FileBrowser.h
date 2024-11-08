#ifndef FILE_BROWSER_H
#define FILE_BROWSER_H

#include <duin.h>

#include <vector>
#include <string>
#include <filesystem>

namespace fs = std::filesystem;

typedef struct FileExplorer {
    fs::path rootPath;                              // Project root directory
    fs::path currentPath;                           // Current directory path
    std::vector<fs::directory_entry> entries;        // Entries in the current directory
    std::vector<fs::path> selectedItems;            // Selected items (optional)
} FileExplorer;

void FileExplorer_Init(FileExplorer* explorer, const fs::path& root);
void FileExplorer_Refresh(FileExplorer* explorer); 
void FileExplorer_EnterDirectory(FileExplorer* explorer, const fs::path& dir); 
void FileExplorer_GoUp(FileExplorer* explorer); 
std::string FileExplorer_GetRelativePath(const FileExplorer* explorer, const fs::path& path); 
void RenderFileExplorer(FileExplorer* explorer);

#endif /* FILE_BROWSER_H */
