#include "StateStartupMenu.h"
#include "../Singletons.h"
#include "../FileBrowser.h"
#include "States.h"

#include <nfd.h>


#define MAX_DIR_LEN 512

const char *EDITOR_CFG_PATH = "./data/editor_cfg.json\0";
FileExplorer explorer;






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
    dbgConsole.LogEx(duin::LogLevel::Info, "ENTERING STARTUPMENU");
    NFD_Init();
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

void StateStartupMenu::LoadCurrentProject()
{
    rapidjson::Document doc;
    if (ReadJSONFile(EDITOR_CFG_PATH, &doc)) {
            dbgConsole.LogEx(duin::LogLevel::Warn, 
                             "EditorConfig empty. Select project root.\n");
        return;
    }

    // Ensure "EditorConfig" exists and is an object
    if (!doc.HasMember("EditorConfig") || !doc["EditorConfig"].IsObject()) {
            dbgConsole.LogEx(duin::LogLevel::Warn, 
                             "EditorConfig not found. Select project root.\n");
        return;
    }

    const rapidjson::Value& editorConfig = doc["EditorConfig"];

    // Ensure "CurrentProjectDir" exists and is a string
    if (!editorConfig.HasMember("CurrentProjectDir") 
        || !editorConfig["CurrentProjectDir"].IsString()) {
            dbgConsole.LogEx(duin::LogLevel::Warn, 
                             "Project directory not found. Select project root.\n");
        return;
    }

    std::string currentProjectDir = 
        editorConfig["CurrentProjectDir"].GetString();

    if (!currentProjectDir.empty()) {
        fs::path projectPath = currentProjectDir;
        if (fs::exists(projectPath) && fs::is_directory(projectPath)) {
            dbgConsole.LogEx(duin::LogLevel::Info, 
                             "Project loaded: %s", projectPath.string().c_str());
        } else {
            dbgConsole.LogEx(duin::LogLevel::Warn, 
                             "Project directory invalid. Select project root.\n");
        }
    } else {
            dbgConsole.LogEx(duin::LogLevel::Warn, 
                             "Project directory empty. Select project root.\n");
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
        LoadCurrentProject();
        owner.SwitchState<StateLevelEditor>();
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
    // show the dialog
    nfdopendialogu8args_t args = {0};
    args.filterList = filterItem;
    args.filterCount = 1;
    nfdresult_t result = NFD_OpenDialogU8_With(&outPath, &args);
    if (result == NFD_OKAY) {
        dbgConsole.LogEx(duin::LogLevel::Info, 
                         "Success: %s", outPath);
        // remember to free the memory (since NFD_OKAY is returned)
        importedProjPath = std::string(outPath);
        NFD_FreePath(outPath);
    } else if (result == NFD_CANCEL) {
        dbgConsole.LogEx(duin::LogLevel::Info, 
                         "Canceled");
        return;
    } else {
        dbgConsole.LogEx(duin::LogLevel::Warn, 
                         "Error: %s", NFD_GetError());
        return;
    }


}
