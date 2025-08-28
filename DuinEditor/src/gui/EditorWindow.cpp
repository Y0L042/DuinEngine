#include "EditorWindow.h"

#include "../Project.h"
#include "../Singletons.h"
#include "GuiMeta.h"

#include <Duin/Core/Debug/DebugModule.h>
#include <Duin/Core/Application.h>

static const char RENAME_POPUP[] = "RENAME_POPUP";
static bool requestRenamePopupFlag = false;
static int renamingTabIndex = -1; // Currently renaming tab (-1 means none)
static char renamingBuffer[256] = {}; // Buffer for renaming
static ImVec2 popupPos;

static const char CONFIRM_TAB_DELETE[] = "CONFIRM_TAB_DELETE";
static bool requestDeleteTabFlag = false;
static int requestDeleteIndex = -1;

EditorWindow::EditorWindow()
{}

void EditorWindow::Init(duin::JSONValue value)
{
    if (value.IsValid())
    {
        Deserialise(value);
        if (tabs.empty())
        {
            DN_WARN("No tabs found, adding tab!");
            CreateAndAddTab("Editor");
            DN_WARN("Tab added.");
        }
    }
    else
    {
        DN_WARN("Initialising with no tab data, adding tab!");
        if (tabs.empty())
        {
            CreateAndAddTab("Editor");
        }
    }
}

std::shared_ptr<Tab> EditorWindow::CreateTab(const std::string& title)
{    
    static int counter = 1;
    bool contains = false;
    std::string newTitle;
    for (auto& tab : tabs) {
        contains = contains || !title.compare(tab->title);
    }
    if (contains) {
        newTitle = title + "_" + std::to_string(++counter);
    } else {
        newTitle = title;
    }
    std::shared_ptr<Tab> newTab = Tab::Create(this);
    if (newTitle != "__NO_TITLE__")
    {
	    newTab->SetTitle(newTitle);
    }

	return newTab;
}

void EditorWindow::AddTab(std::shared_ptr<Tab> tab)
{
    tabs.emplace_back(tab);
}

void EditorWindow::CreateAndAddTab(const std::string& title)
{
    std::shared_ptr<Tab> newTab = CreateTab(title);
	tabs.emplace_back(newTab);
}

void EditorWindow::CloseTab(int index) 
{
    if (index < 0 || index >= tabs.size()) return;
    
    tabs.erase(tabs.begin() + index);
    if (selectedTab >= tabs.size()) {
        selectedTab = tabs.size() - 1;
    }
    if (tabs.empty()) {
        CreateAndAddTab("New Tab");
    }
}

void EditorWindow::Render()
{
    ImGuiViewport* viewport = ImGui::GetMainViewport();
    ImGui::SetNextWindowPos(viewport->Pos);
    ImGui::SetNextWindowSize(viewport->Size);
    ImGui::SetNextWindowViewport(viewport->ID);

    DrawMainTabBrowser();
}

void EditorWindow::DrawMainTabBrowser()
{
    ImGuiWindowFlags windowFlags =
        ImGuiWindowFlags_NoCollapse |
        ImGuiWindowFlags_NoResize |
        ImGuiWindowFlags_NoMove |
        ImGuiWindowFlags_NoBringToFrontOnFocus |
        ImGuiWindowFlags_NoNavFocus |
        ImGuiWindowFlags_NoDocking |
        ImGuiWindowFlags_MenuBar
        ;
    const std::string tabBrowserName = "Active Project: " + GetActiveProject().projectTitle;

    if (ImGui::Begin(tabBrowserName.c_str(), nullptr, windowFlags)) {
        DrawMenu();
        DrawTabBar();
        DrawTabContentArea();
        DrawRenameTabPopup();
        DrawConfirmDeleteTabPopup();

    } 
    ImGui::End();
}

void EditorWindow::DrawTabBar()
{
    ImGuiWindowFlags tabBarFlags = ImGuiWindowFlags_NoScrollbar | ImGuiWindowFlags_NoScrollWithMouse;
    if (ImGui::BeginChild("TabBarChild", ImVec2(0, 25), 0, tabBarFlags)) {

        /* Draw tab bar */
        ImGuiWindowFlags tabBarFlags = ImGuiTabBarFlags_AutoSelectNewTabs | 
                                       ImGuiTabBarFlags_Reorderable | 
                                       ImGuiTabBarFlags_FittingPolicyScroll |
                                       ImGuiTabBarFlags_NoCloseWithMiddleMouseButton;

        if (ImGui::BeginTabBar("MainTabBar", tabBarFlags)) {
            // Render each tab
            for (int i = 0; i < tabs.size(); ++i) {
                auto& tab = tabs[i];
                ImGuiTabItemFlags flags = 0;

                /* Draw tab item */
                bool deleteTab = !tab->open;
                bool tabOpen = tab->open;
                if (ImGui::BeginTabItem(tab->title.c_str(), &tabOpen, flags)) {
                    selectedTab = i;

                    /* DOUBLE CLICK ON TAB EVENT */
                    if (ImGui::IsMouseDoubleClicked(0) && ImGui::IsItemHovered()) {
                        //DN_INFO("Attempting to rename tab: {}", tab.title.c_str());
						requestRenamePopupFlag = true;
                        renamingTabIndex = i;
                        strncpy(renamingBuffer, tab->title.c_str(), sizeof(renamingBuffer));
                        popupPos = { 0, 0 };
                    }
                    ImGui::EndTabItem();
                }

                if (!tabOpen && !deleteTab) {
                    requestDeleteIndex = i;
                    requestDeleteTabFlag = true;
                    tab->flag_requestDelete = true;
                }

                if (deleteTab) {
                    if (!tab->open) {
                        CloseTab(i);
                        if (i > 0) i--;
                    }
                }

                tab->SetFocussed(i == selectedTab);
            }

            // Add a "+" button to create new tabs
            if (ImGui::TabItemButton("+", ImGuiTabItemFlags_Trailing | ImGuiTabItemFlags_NoTooltip)) {
                CreateAndAddTab("New Tab");
            }
            ImGui::EndTabBar();
        }
    }
    ImGui::EndChild();
}

void EditorWindow::DrawTabContentArea()
{
    if (selectedTab >= 0 && selectedTab < tabs.size()) {
        auto& tab = tabs[selectedTab];
        tab->DrawWorkspace();
    }
}

void EditorWindow::DrawRenameTabPopup()
{
    // Check if we need to open the popup this frame
    if (requestRenamePopupFlag) {
        requestRenamePopupFlag = false;
		//DN_INFO("Opening rename tab popup at index: {}", renamingTabIndex);
        ImGui::OpenPopup(RENAME_POPUP);
        ImGui::SetNextWindowFocus();
    }

    if (ImGui::BeginPopup(RENAME_POPUP)) {
        ImGui::SetNextItemWidth(150.0f);

        ImGuiInputTextFlags inputFlags = ImGuiInputTextFlags_EnterReturnsTrue | 
                                         ImGuiInputTextFlags_AutoSelectAll;

        if (ImGui::InputText("##RenameTab", renamingBuffer, sizeof(renamingBuffer), inputFlags)) {
            if (renamingTabIndex >= 0 && renamingTabIndex < (int)tabs.size()) {
                tabs[renamingTabIndex]->title = renamingBuffer;
            }
            renamingTabIndex = -1;
            ImGui::CloseCurrentPopup();
        }

        if (ImGui::IsKeyPressed(ImGuiKey_Escape) || ImGui::IsMouseClicked(1)) {
            renamingTabIndex = -1;
            ImGui::CloseCurrentPopup();
        }

        ImGui::EndPopup();
    } else if (renamingTabIndex >= 0) {
		DN_WARN("Popup closed without renaming tab");
        renamingTabIndex = -1;
    }
}

bool EditorWindow::DrawConfirmDeleteTabPopup()
{
    bool actionDelete = false;
    ImGuiWindowFlags deletePopupFlags = 0;
    if (requestDeleteTabFlag) {
        requestDeleteTabFlag = false;
        //DN_INFO("Requestiong tab delete");
        ImGui::OpenPopup(CONFIRM_TAB_DELETE);
        ImGui::SetNextWindowFocus();
    }
    if (ImGui::BeginPopupModal(CONFIRM_TAB_DELETE)) {
        //DN_INFO("Popup tab delete");
        // Delete tab
        if (ImGui::Button("Delete") && (requestDeleteIndex >= 0 && requestDeleteIndex < (int)tabs.size())) {
            tabs[requestDeleteIndex]->flag_requestDelete = false;
            tabs[requestDeleteIndex]->open = false;
            actionDelete = true;
            requestDeleteIndex = -1;
            requestDeleteTabFlag = false;
            ImGui::CloseCurrentPopup();
        } else
        // Keep tab open
        if (ImGui::Button("Cancel") && (requestDeleteIndex >= 0 && requestDeleteIndex < (int)tabs.size())) {
            tabs[requestDeleteIndex]->flag_requestDelete = false;
            tabs[requestDeleteIndex]->open = true;
            actionDelete =  false;
            requestDeleteIndex = -1;
            requestDeleteTabFlag = false;
            ImGui::CloseCurrentPopup();
        }   
        ImGui::EndPopup();
    }
    else if (requestDeleteIndex >= 0) {
        requestDeleteIndex = -1;
    }

    return actionDelete;
}


duin::JSONValue EditorWindow::Serialise()
{
    duin::JSONValue tabsArray;
    tabsArray.SetArray();

    for (auto& tab : tabs) {
        DN_INFO("Saving tab into tabsArray");
        tabsArray.PushBack(tab->Serialise());
    }

    duin::JSONValue value;
    value.AddMember(guitag::TABS_KEY, tabsArray);

    return value;
}

void EditorWindow::Deserialise(duin::JSONValue data)
{
    if (!(data.HasMember(guitag::TABS_KEY) && data[guitag::TABS_KEY].IsArray())) {
        DN_WARN("EditorWindow passed empty tabs array!");
        return;
    }

    duin::JSONValue tabsArray = data[guitag::TABS_KEY];
    tabs.clear();
    for (auto item : tabsArray) {
        DN_INFO("Tab item: \n{}\n", duin::JSONValue::Write(item));
        auto tab = CreateTab();
		tab->Deserialise(item);
        AddTab(tab);
    }
}

duin::UUID EditorWindow::GetUUID()
{
    return uuid;
}

extern void SaveProjectConfig();
void EditorWindow::DrawMenu()
{
    if (ImGui::BeginMenuBar()) {
        // FILE
        if (ImGui::BeginMenu("File")) {
            if (ImGui::MenuItem("TODO")) {
            }
            if (ImGui::MenuItem("TODO2")) {
            }
            ImGui::EndMenu();
        }

        // EDITOR
        if (ImGui::BeginMenu("Editor")) {
            if (ImGui::MenuItem("Save Layout")) {
                SaveProjectConfig();
            }
            if (ImGui::MenuItem("TODO2")) {
            }
            ImGui::EndMenu();
        }

        ImGui::EndMenuBar();
    }
}
