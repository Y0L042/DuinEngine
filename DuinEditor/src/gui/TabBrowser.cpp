#include "TabBrowser.h"

#include "../Project.h"
#include "../Singletons.h"
#include "GuiMeta.h"

#include <Duin/Core/Debug/DebugModule.h>
#include <Duin/Core/Utils/TOMLFile.h>

static const char RENAME_POPUP[] = "RENAME_POPUP";
static bool requestRenamePopupFlag = false;
static int renamingTabIndex = -1; // Currently renaming tab (-1 means none)
static char renamingBuffer[256] = {}; // Buffer for renaming
static ImVec2 popupPos;

static const char CONFIRM_TAB_DELETE[] = "CONFIRM_TAB_DELETE";
static bool requestDeleteTabFlag = false;
static int requestDeleteIndex = -1;

TabBrowser::TabBrowser()
{}

void TabBrowser::Init()
{
    if (tabs.empty()) {
        DN_WARN("No tabs found, adding tab!");
        AddTab("Editor");
    }
}

void TabBrowser::Init(duin::TOMLValue value)
{
    Deserialise(value);
    if (tabs.empty()) {
        DN_WARN("No tabs found, adding tab!");
        AddTab("Editor");
    }
}

void TabBrowser::AddTab(const std::string& title)
{    
    static int counter = 1;
    Tab newTab;

    bool contains = false;
    for (Tab& tab : tabs) {
        contains = contains || !title.compare(tab.title);
    }
    if (contains) {
        newTab.title = title + "_" + std::to_string(++counter);
    } else {
        newTab.title = title;
    }
    CreateTab(newTab);

    SaveProjectConfig();
}

void TabBrowser::CreateTab(Tab newTab)
{
    tabs.push_back(newTab);
}

void TabBrowser::CloseTab(int index) 
{
    if (index < 0 || index >= tabs.size()) return;
    
    tabs.erase(tabs.begin() + index);
    if (selectedTab >= tabs.size()) {
        selectedTab = tabs.size() - 1;
    }
    if (tabs.empty()) {
        AddTab("New Tab");
    }

    SaveProjectConfig();
}

void TabBrowser::Render()
{
    ImGuiViewport* viewport = ImGui::GetMainViewport();
    ImGui::SetNextWindowPos(viewport->Pos);
    ImGui::SetNextWindowSize(viewport->Size);
    ImGui::SetNextWindowViewport(viewport->ID);

    DrawMainTabBrowser();
}

void TabBrowser::DrawMainTabBrowser()
{
    ImGuiWindowFlags windowFlags =
        ImGuiWindowFlags_NoCollapse |
        ImGuiWindowFlags_NoResize |
        ImGuiWindowFlags_NoMove |
        ImGuiWindowFlags_NoBringToFrontOnFocus |
        ImGuiWindowFlags_NoNavFocus |
        ImGuiWindowFlags_NoDocking 
        ;
    const std::string tabBrowserName = "Active Project: " + GetActiveProject().projectTitle;

    if (ImGui::Begin(tabBrowserName.c_str(), nullptr, windowFlags)) {
        DrawTabBar();
        DrawTabContentArea();
        DrawRenameTabPopup();
        DrawConfirmDeleteTabPopup();

    } 
    ImGui::End();
}

void TabBrowser::DrawTabBar()
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
                Tab& tab = tabs[i];
                ImGuiTabItemFlags flags = 0;

                /* Draw tab item */
                bool deleteTab = !tab.open;
                bool tabOpen = tab.open;
                if (ImGui::BeginTabItem(tab.title.c_str(), &tabOpen, flags)) {
                    selectedTab = i;

                    /* DOUBLE CLICK ON TAB EVENT */
                    if (ImGui::IsMouseDoubleClicked(0) && ImGui::IsItemHovered()) {
                        //DN_INFO("Attempting to rename tab: {}", tab.title.c_str());
						requestRenamePopupFlag = true;
                        renamingTabIndex = i;
                        strncpy(renamingBuffer, tab.title.c_str(), sizeof(renamingBuffer));
                        popupPos = { 0, 0 };
                    }
                    ImGui::EndTabItem();
                }

                if (!tabOpen && !deleteTab) {
                    requestDeleteIndex = i;
                    requestDeleteTabFlag = true;
                    tab.flag_requestDelete = true;
                }

                if (deleteTab) {
                    if (!tab.open) {
                        CloseTab(i);
                        if (i > 0) i--;
                    }
                }
            }

            // Add a "+" button to create new tabs
            if (ImGui::TabItemButton("+", ImGuiTabItemFlags_Trailing | ImGuiTabItemFlags_NoTooltip)) {
                AddTab("New Tab");
            }
            ImGui::EndTabBar();
        }
    }
    ImGui::EndChild();
}

void TabBrowser::DrawTabContentArea()
{
    if (selectedTab >= 0 && selectedTab < tabs.size()) {
        Tab& tab = tabs[selectedTab];
        tab.DrawWorkspace();
    }
}

void TabBrowser::DrawRenameTabPopup()
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
                tabs[renamingTabIndex].title = renamingBuffer;
                SaveProjectConfig();
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

bool TabBrowser::DrawConfirmDeleteTabPopup()
{
    bool actionDelete = false;
    ImGuiWindowFlags deletePopupFlags = ImGuiWindowFlags_AlwaysAutoResize;
    if (requestDeleteTabFlag) {
        requestDeleteTabFlag = false;
        //DN_INFO("Requestiong tab delete");
        ImGui::OpenPopup(CONFIRM_TAB_DELETE);
        ImGui::SetNextWindowFocus();
    }
    if (ImGui::BeginPopup(CONFIRM_TAB_DELETE)) {
        //DN_INFO("Popup tab delete");
        // Delete tab
        if (ImGui::Button("Delete") && (requestDeleteIndex >= 0 && requestDeleteIndex < (int)tabs.size())) {
            tabs[requestDeleteIndex].flag_requestDelete = false;
            tabs[requestDeleteIndex].open = false;
            actionDelete = true;
            requestDeleteIndex = -1;
            requestDeleteTabFlag = false;
            ImGui::CloseCurrentPopup();
        } else
        // Keep tab open
        if (ImGui::Button("Cancel") && (requestDeleteIndex >= 0 && requestDeleteIndex < (int)tabs.size())) {
            tabs[requestDeleteIndex].flag_requestDelete = false;
            tabs[requestDeleteIndex].open = true;
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


duin::TOMLValue TabBrowser::Serialise()
{
    duin::TOMLValue tomlValue;
    toml::value& value = tomlValue.GetValue();
    value[guitag::TABS_KEY] = toml::array{};
    auto& tabsArray = value[guitag::TABS_KEY].as_array();
    tabsArray.clear();

    for (Tab& tab : tabs) {
        tabsArray.push_back(tab.Serialise().GetValue());
    }

    //file.at(guitag::TABS_KEY).as_array_fmt().fmt = toml::array_format::multiline;
    //file.at(guitag::TABS_KEY).as_array_fmt().indent_type = toml::indent_char::space;
    //file.at(guitag::TABS_KEY).as_array_fmt().body_indent = 2;
    
    return tomlValue;
}

void TabBrowser::Deserialise(duin::TOMLValue tomlValue)
{
    toml::value& file = tomlValue.GetValue();
    if (file.contains(guitag::TABS_KEY)) {
        auto& tabsArray = file[guitag::TABS_KEY].as_array();
        tabs.clear();

        for (auto& item : tabsArray) {
            duin::TOMLValue value(item);
            Tab tab(value);
            tabs.push_back(tab);
        }
    }
}

duin::UUID TabBrowser::GetUUID()
{
    return uuid;
}
