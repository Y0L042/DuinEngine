#include "State_SelectProject.h"
#include <Duin/Core/Debug/DNLog.h>
#include <Duin/IO/IOModule.h>
#include <Duin/Core/Debug/DebugModule.h>
#include <Duin/Core/Application.h>
#include <external/imgui.h>

#include <nfd.h>
#include <iostream>
#include <vector>
#include <string>

#include "FileManager.h"
#include "Project.h"
#include "Singleton.h"
#include "States.h"

void State_SelectProject::Enter()
{
    InitProjectList();
}

void State_SelectProject::OnEvent(duin::Event e)
{
}

void State_SelectProject::Update(double delta)
{
}

void State_SelectProject::PhysicsUpdate(double delta)
{
}

void State_SelectProject::Draw()
{
}

void State_SelectProject::DrawUI()
{
    DrawGUI();
}

void State_SelectProject::Exit()
{
    NFD_Quit();
}

void State_SelectProject::InitProjectList()
{
    DN_INFO("Initialising Project List.");

    // Read editor config

    duin::ConfigValue file = duin::ConfigValue::Parse(EDITOR_CFG_PATH);
    if (file.IsEmpty())
    {
        DN_WARN("EditorConfig empty. Select project root.");
        return;
    }

    // Ensure "EditorConfig" exists and is an object
    if (!file.Contains("EditorConfig"))
    {
        DN_WARN("EditorConfig not found. Select project root.");
        return;
    }
    DN_INFO("EditorConfig loaded.");

    // Read "RecentProjectDirs"
    recentProjectDirsVec =
        duin::ConfigValue::Get<std::vector<std::string>>(file.At("EditorConfig").At("RecentProjectDirs"));
    DN_INFO("RecentProjectDirs:");
    for (int i = 0; i < recentProjectDirsVec.size(); ++i)
    {
        DN_INFO("{0}", recentProjectDirsVec[i]);
    }
}

void State_SelectProject::LoadEditorCFG()
{
}

void State_SelectProject::ImportProject()
{
    std::string importedProjPath;
    nfdchar_t *outPath;
    nfdfilteritem_t filterItem[1] = {{"Duin Project", "dnproj"}};
    nfdopendialogu8args_t args = {0};
    args.filterList = filterItem;
    args.filterCount = 1;
    nfdresult_t result = NFD_OpenDialogU8_With(&outPath, &args);
    if (result == NFD_OKAY)
    {
        DN_INFO("Success: {0}", outPath);
        importedProjPath = std::string(outPath);
        NFD_FreePath(outPath);
    }
    else if (result == NFD_CANCEL)
    {
        DN_INFO("Canceled");
        return;
    }
    else
    {
        DN_WARN("Error: {0}", NFD_GetError());
        return;
    }
}

void State_SelectProject::LoadSelectedProject()
{
    if (selectedProjectIndex == -1)
    {
        return;
    }

    FileManager &fileManager = FileManager::Get();

    const char *selectedProjectDir = recentProjectDirsVec[selectedProjectIndex].c_str();
    fs::path projectPath = selectedProjectDir;
    // If project dir exist
    if (fs::exists(projectPath) && fs::is_directory(projectPath))
    {
        int fileFound = 0;
        // Search for the duin.project file in the directory
        for (const auto &entry : fs::directory_iterator(projectPath))
        {
            if (entry.is_regular_file() && !entry.path().filename().string().compare(PROJECT_FILE_NAME))
            {
                fileFound = true;

                // Log the full path of the found file
                DN_INFO("Project loaded: {0}, File found: {1}", projectPath.string(), entry.path().string());

                Project activeProject(entry.path());
                activeProject.projectDir = entry.path();
                DN_INFO("Project loaded: {0}", activeProject.projectDir.string());
                Singleton::SetActiveProject(activeProject);

                fileManager.SetRootPath(projectPath.string());
                duin::SetImGuiINIPath(projectPath.string() + "/imgui.ini"); // Set per-project imgui ini

                SwitchState<State_SceneEditor>();
                DN_INFO("Switching to Editor State");
                break;
            }
        }

        if (!fileFound)
        {
            DN_WARN("File '{0}' not found in the selected project directory: {1}", PROJECT_FILE_NAME,
                    projectPath.string());
        }
    }
    else
    {
        DN_WARN("Project directory invalid. Select project root.");
    }
}

void State_SelectProject::DrawGUI()
{
    ImGui::Begin("Menu");
    ImGui::BeginChild("Projects", {0, 0}, 0, 0);
    if (ImGui::Button("Import Project"))
    {
        ImportProject();
    }
    ImGui::SameLine();

    if (ImGui::Button("Load Project"))
    {
        DN_INFO("Loading project...");
        LoadSelectedProject();
    }
    for (size_t i = 0; i < recentProjectDirsVec.size(); ++i)
    {
        bool projectIsSelected = ImGui::Selectable(recentProjectDirsVec[i].c_str(), selectedProjectIndex == i);
        if (projectIsSelected)
        {
            selectedProjectIndex = i;
            DN_INFO("Project[{0}] {{{1}}} selected", i, recentProjectDirsVec[i]);
        }
        if (ImGui::IsItemHovered() && ImGui::IsMouseDoubleClicked(0))
        {
            LoadSelectedProject();
            break;
        }
    }
    ImGui::EndChild();

    ImGui::BeginChild("Menu", {0, 0}, 0, 0);
    ImGui::EndChild();
    ImGui::End();
}
