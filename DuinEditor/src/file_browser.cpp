#include "file_browser.h"



// Initialize the FileExplorer with the given root path
void FileExplorer_Init(FileExplorer* explorer, const fs::path& root) 
{
    explorer->rootPath = root;
    explorer->currentPath = root;
    // Populate the entries
    FileExplorer_Refresh(explorer);
}



// Refresh the directory entries in the current path
void FileExplorer_Refresh(FileExplorer* explorer) 
{
    explorer->entries.clear();
    if (fs::exists(explorer->currentPath) && fs::is_directory(explorer->currentPath)) {
        for (const auto& entry : fs::directory_iterator(explorer->currentPath)) {
            explorer->entries.push_back(entry);
        }
    }
}



// Enter a subdirectory within the current path
void FileExplorer_EnterDirectory(FileExplorer* explorer, const fs::path& dir) 
{
    explorer->currentPath /= dir;
    FileExplorer_Refresh(explorer);
}



// Navigate to the parent directory
void FileExplorer_GoUp(FileExplorer* explorer) 
{
    if (explorer->currentPath.has_parent_path() 
            && explorer->currentPath != explorer->rootPath) {
        explorer->currentPath = explorer->currentPath.parent_path();
        FileExplorer_Refresh(explorer);
    }
}



// Get the relative path from the root directory
std::string FileExplorer_GetRelativePath(const FileExplorer* explorer, const fs::path& path) 
{
    return fs::relative(path, explorer->rootPath).string();
}



// Render the File Explorer window
void RenderFileExplorer(FileExplorer* explorer) {
    ImGui::Begin("File Explorer");

    // Display the current path relative to root
    std::string relativePath = FileExplorer_GetRelativePath(explorer, explorer->currentPath);
    ImGui::Text("Current Path: %s", relativePath.c_str());

    // Navigation buttons
    if (ImGui::Button("Up")) {
        FileExplorer_GoUp(explorer);
    }
    ImGui::SameLine();
    if (ImGui::Button("Refresh")) {
        FileExplorer_Refresh(explorer);
    }

    ImGui::Separator();

    // Begin child region for scrolling
    ImGui::BeginChild("FileList", ImVec2(0, 0), false, ImGuiWindowFlags_HorizontalScrollbar);

    for (size_t i = 0; i < explorer->entries.size(); ++i) {
        const fs::directory_entry& entry = explorer->entries[i];
        const fs::path& path = entry.path();
        std::string name = path.filename().string();
        bool isDir = entry.is_directory();

        // Set label based on file type
        std::string label = isDir ? "[DIR] " + name : name;

        // Selectable item
        if (ImGui::Selectable(label.c_str(), false, ImGuiSelectableFlags_AllowDoubleClick)) {
            // Handle double-click to enter directory
            if (ImGui::IsMouseDoubleClicked(0) && isDir) {
                FileExplorer_EnterDirectory(explorer, name);
            }
        }

        // Drag and Drop Source (only for files)
        if (!isDir) {
            if (ImGui::BeginDragDropSource(ImGuiDragDropFlags_None)) {
                // Set payload to the relative file path
                std::string payload = FileExplorer_GetRelativePath(explorer, path);
                ImGui::SetDragDropPayload("FILE_PATH", payload.c_str(), payload.size() + 1);
                ImGui::Text("%s", name.c_str());
                ImGui::EndDragDropSource();
            }
        }
    }

    ImGui::EndChild();
    ImGui::End();
}


