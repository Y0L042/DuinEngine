#include "StateStartupMenu.h"
#include "../Singletons.h"
#include "../FileManager.h"
#include "States.h"

#include <nfd.h>
#include <iostream>
#include <vector>
#include <string>


#define MAX_DIR_LEN 512

const char *EDITOR_CFG_PATH = "./data/editor_cfg.json\0";
const char *PROJECT_FILE_NAME = "duin.project\0";

static std::vector<std::string> recentProjectDirsVec;
static int selectedProjectIndex = -1;




StateStartupMenu::StateStartupMenu(duin::GameStateMachine& owner)
    : GameState(owner)
{
    Enter = [this]() { State_Enter(); };
    Exit = [this]() { State_Exit(); };
    HandleInput = [this]() { State_HandleInput(); };
    Update = [this](double delta) { State_Update(delta); };
    PhysicsUpdate = [this](double delta) { State_PhysicsUpdate(delta); };
    Draw = [this]() { State_Draw(); };
    DrawUI = [this]() { State_DrawUI(); };
}

StateStartupMenu::~StateStartupMenu()
{}



void StateStartupMenu::State_Enter()
{
    debugConsole.LogEx(duin::LogLevel::Info, "ENTERING STARTUPMENU");
    NFD_Init();

    InitProjectList();
}

void StateStartupMenu::State_Exit()
{
    NFD_Quit();
}

void StateStartupMenu::State_HandleInput()
{}

void StateStartupMenu::State_Update(double delta)
{}

void StateStartupMenu::State_PhysicsUpdate(double delta)
{}

void StateStartupMenu::State_Draw()
{}

void StateStartupMenu::State_DrawUI()
{
    DrawGUI();
}

void StateStartupMenu::InitProjectList()
{
    debugConsole.LogEx(duin::LogLevel::Info, 
                     "Initialising Project List.\n");
    duin::JSONDocument doc;
    if (ReadJSONFile(EDITOR_CFG_PATH, doc)) {
            debugConsole.LogEx(duin::LogLevel::Warn, 
                             "EditorConfig empty. Select project root.\n");
        return;
    }

    // Ensure "EditorConfig" exists and is an object
    if (!doc.HasMember("EditorConfig") || !doc.GetDocument()["EditorConfig"].IsObject()) {
            debugConsole.LogEx(duin::LogLevel::Warn, 
                             "EditorConfig not found. Select project root.\n");
        return;
    }
    const rapidjson::Value& editorConfig = doc.GetDocument()["EditorConfig"];
    debugConsole.LogEx(duin::LogLevel::Info, 
                     "EditorConfig loaded.\n");

    // Read "RecentProjectDirs"
    if (editorConfig.HasMember("RecentProjectDirs") && editorConfig["RecentProjectDirs"].IsArray()) {
        const rapidjson::Value& recentDirs = editorConfig["RecentProjectDirs"];

        debugConsole.LogEx(duin::LogLevel::Info, 
                         "RecentProjectDirs:");

        for (rapidjson::SizeType i = 0; i < recentDirs.Size(); ++i) {
            if (recentDirs[i].IsString()) {
                recentProjectDirsVec.push_back(recentDirs[i].GetString());

                debugConsole.LogEx(duin::LogLevel::Info, 
                                 "%s", recentDirs[i].GetString());
            }
        }
    } else {
        debugConsole.LogEx(duin::LogLevel::Error, 
                         "RecentProjectDirs is missing or not an array.");
    }
}

void StateStartupMenu::LoadSelectedProject()
{
    if (selectedProjectIndex == -1) { return; }

    const char *selectedProjectDir = recentProjectDirsVec[selectedProjectIndex].c_str();

    fs::path projectPath = selectedProjectDir;
    if (fs::exists(projectPath) && fs::is_directory(projectPath)) {
        int fileFound = 0;
        // Search for the file in the directory
        for (const auto& entry : fs::directory_iterator(projectPath)) {
            if (entry.is_regular_file() && entry.path().filename() == PROJECT_FILE_NAME) {
                fileFound = true;

                // Log the full path of the found file
                debugConsole.LogEx(duin::LogLevel::Info,
                                 "Project loaded: %s, File found: %s", 
                                 projectPath.string().c_str(), 
                                 entry.path().string().c_str());

                SetActiveProject(entry.path().string());
                fileManager.rootPath = projectPath.string();

                owner.SwitchState<StateLevelEditor>();
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

void StateStartupMenu::DrawGUI()
{
    ImGui::Begin("Menu");
    ImGui::BeginChild("Projects", {0, 0}, 0, 0);
    if (ImGui::Button("Import Project")) {
        ImportProject();
    }
    ImGui::SameLine();
    if (ImGui::Button("Load Project")) {
        LoadSelectedProject();
    }
    for (size_t i = 0; i < recentProjectDirsVec.size(); ++i) {
        bool projectIsSelected = ImGui::Selectable(recentProjectDirsVec[i].c_str(), selectedProjectIndex == i);
        if (projectIsSelected) {
            selectedProjectIndex = i;
            debugConsole.LogEx(duin::LogLevel::Info, 
                             "Project[%d] {%s} selected", i, recentProjectDirsVec[i].c_str());
        }
    }
    ImGui::EndChild();

    ImGui::BeginChild("Menu", {0, 0}, 0, 0);
    ImGui::EndChild();
    ImGui::End();
}

void StateStartupMenu::LoadEditorCFG()
{}

void StateStartupMenu::ImportProject()
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
