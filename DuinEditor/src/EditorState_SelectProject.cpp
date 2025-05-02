#include "EditorState_SelectProject.h"

#include <Duin/IO/IOModule.h>
#include <Duin/Core/Debug/DebugModule.h>
#include <Duin/Core/Application.h>
#include <external/imgui.h>

#include <nfd.h>
#include <iostream>
#include <vector>
#include <string>

#include "Project.h"
#include "Singletons.h"
#include "gui/FileManager.h"
#include "States.h"

#define MAX_DIR_LEN 512

const std::string EDITOR_CFG_PATH = "data/editor_cfg.toml";
const std::string PROJECT_FILE_NAME = "duin.project";

static std::vector<std::string> recentProjectDirsVec;
static int selectedProjectIndex = -1;


void EditorState_SelectProject::Enter()
{
    debugConsole.LogEx(duin::LogLevel::Info, "ENTERING STARTUPMENU");
    NFD_Init();

    InitProjectList();
}

void EditorState_SelectProject::OnEvent(duin::Event e)
{
}

void EditorState_SelectProject::Update(double delta)
{
}

void EditorState_SelectProject::PhysicsUpdate(double delta)
{
}

void EditorState_SelectProject::Draw()
{
}

void EditorState_SelectProject::DrawUI()
{
    DrawGUI();
}

void EditorState_SelectProject::Exit()
{
    NFD_Quit();
}

void EditorState_SelectProject::InitProjectList()
{
    debugConsole.LogEx(duin::LogLevel::Info, "Initialising Project List.\n");

    // Read editor config

    duin::ConfigValue file = duin::ConfigValue::Parse(EDITOR_CFG_PATH);
    if (file.IsEmpty()) {
        debugConsole.LogEx(duin::LogLevel::Warn, "EditorConfig empty. Select project root.\n");
        return;
    }

    // Ensure "EditorConfig" exists and is an object
    if (!file.Contains("EditorConfig")) {
        debugConsole.LogEx(duin::LogLevel::Warn, "EditorConfig not found. Select project root.\n");
        return;
    }
    debugConsole.LogEx(duin::LogLevel::Info, 
                     "EditorConfig loaded.\n");

    // Read "RecentProjectDirs"
    recentProjectDirsVec = duin::ConfigValue::Get<std::vector<std::string>>(file.At("EditorConfig").At("RecentProjectDirs"));
    debugConsole.LogEx(duin::LogLevel::Info, "RecentProjectDirs:");
    for (int i = 0; i < recentProjectDirsVec.size(); ++i) {
        debugConsole.LogEx(duin::LogLevel::Info, "%s", recentProjectDirsVec[i].c_str());
    }
}

void EditorState_SelectProject::LoadSelectedProject()
{
    if (selectedProjectIndex == -1) { return; }

    const char *selectedProjectDir = recentProjectDirsVec[selectedProjectIndex].c_str();
    fs::path projectPath = selectedProjectDir;
    // If project dir exist
    if (fs::exists(projectPath) && fs::is_directory(projectPath)) {
        int fileFound = 0;
        // Search for the duin.project file in the directory
        for (const auto& entry : fs::directory_iterator(projectPath)) {
            if (entry.is_regular_file() && !entry.path().filename().string().compare(PROJECT_FILE_NAME)) {
                fileFound = true;

                // Log the full path of the found file
                debugConsole.LogEx(duin::LogLevel::Info,
                                 "Project loaded: %s, File found: %s", 
                                 projectPath.string().c_str(), 
                                 entry.path().string().c_str());

                Project activeProject(entry.path());
                activeProject.projectDir = entry.path(); 
                DN_INFO("Project loaded: {0}", activeProject.projectDir.string());
                SetActiveProject(activeProject);

                fileManager.rootPath = projectPath.string();
                duin::SetImGuiINIPath(projectPath.string() + "/imgui.ini"); // Set per-project imgui ini

                SwitchState<EditorState_GameEditor>();
                break;
            }
        }

        if (!fileFound) {
            debugConsole.LogEx(duin::LogLevel::Warn, 
                             "File '%s' not found in the selected project directory: %s", 
                             PROJECT_FILE_NAME,
                             projectPath.string().c_str());
        }
    } else {
        debugConsole.LogEx(duin::LogLevel::Warn, 
                         "Project directory invalid. Select project root.\n");
    }
}

void EditorState_SelectProject::DrawGUI()
{
    ImGui::Begin("Menu");
    ImGui::BeginChild("Projects", {0, 0}, 0, 0);
    if (ImGui::Button("Import Project")) {
        ImportProject();
    }
    ImGui::SameLine();

    if (ImGui::Button("Load Project")) {
        debugConsole.LogEx(duin::LogLevel::Info, 
                         "Loading project...");
        LoadSelectedProject();
    }
    for (size_t i = 0; i < recentProjectDirsVec.size(); ++i) {
        bool projectIsSelected = ImGui::Selectable(recentProjectDirsVec[i].c_str(), selectedProjectIndex == i);
        if (projectIsSelected) {
            selectedProjectIndex = i;
            debugConsole.LogEx(duin::LogLevel::Info, 
                             "Project[%d] {%s} selected", i, recentProjectDirsVec[i].c_str());
        }
        if (ImGui::IsItemHovered() && ImGui::IsMouseDoubleClicked(0)) {
            LoadSelectedProject();
            break;
        }
        
    }
    ImGui::EndChild();

    ImGui::BeginChild("Menu", {0, 0}, 0, 0);
    ImGui::EndChild();
    ImGui::End();

    debugWatchlist.Draw("Watchlist");
    debugConsole.Draw("Console");
}

void EditorState_SelectProject::LoadEditorCFG()
{}

void EditorState_SelectProject::ImportProject()
{
    std::string importedProjPath;
    nfdchar_t *outPath;
    nfdfilteritem_t filterItem[1] = {{"Duin Project", "dnproj"}};
    nfdopendialogu8args_t args = {0};
    args.filterList = filterItem;
    args.filterCount = 1;
    nfdresult_t result = NFD_OpenDialogU8_With(&outPath, &args);
    if (result == NFD_OKAY) {
        debugConsole.LogEx(duin::LogLevel::Info, 
                         "Success: %s", outPath);
        importedProjPath = std::string(outPath);
        NFD_FreePath(outPath);
    } else if (result == NFD_CANCEL) {
        debugConsole.LogEx(duin::LogLevel::Info, 
                         "Canceled");
        return;
    } else {
        debugConsole.LogEx(duin::LogLevel::Warn, 
                         "Error: %s", NFD_GetError());
        return;
    }
}
