#include "TabBrowser.h"

#include "../Project.h"
#include "../Singletons.h"
#include "GuiMeta.h"

#include <Duin/Core/Debug/DebugModule.h>
#include <Duin/Core/Utils/TOMLFile.h>

TabBrowser::TabBrowser()
{}

void TabBrowser::Init()
{
    LoadTabs();
    if (tabs.empty()) {
        DN_WARN("No tabs found, adding tab!");
        AddTab("Editor");
    }
}

void TabBrowser::AddTab(const std::string& title)
{    
    static int counter = 1;
    Tab newTab;
    newTab.id = counter++;

    bool contains = false;
    for (Tab& tab : tabs) {
        contains = contains || !title.compare(tab.title);
    }
    if (contains) {
        newTab.title = title + "_" + std::to_string(counter);
    } else {
        newTab.title = title;
    }
    CreateTab(newTab);

    SaveTabs();
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

    SaveTabs();
}

void TabBrowser::Render()
{
    // Begin a dockable window
    if (ImGui::Begin("TabBrowser", nullptr, ImGuiWindowFlags_NoCollapse)) {
        // Tab bar at the top
        if (ImGui::BeginTabBar("MainTabBar", 
                               ImGuiTabBarFlags_AutoSelectNewTabs 
                               | ImGuiTabBarFlags_Reorderable 
                               | ImGuiTabBarFlags_FittingPolicyScroll)) {
            // Render each tab
            for (size_t i = 0; i < tabs.size(); ++i) {
                Tab& tab = tabs[i];
                ImGuiTabItemFlags flags = 0;

                // Rename tab
                if (tab.renaming) {
                    char buffer[256];
                    strncpy(buffer, tab.title.c_str(), sizeof(buffer));
                    if (ImGui::InputText("##RenameTab", buffer, sizeof(buffer), ImGuiInputTextFlags_EnterReturnsTrue)) {
                        tab.title = buffer;
                        tab.renaming = false;
                        SaveTabs();
                    }

                    if (!ImGui::IsItemActive() && ImGui::IsMouseClicked(0)) {
                        tab.renaming = false;
                    }

                    flags |= ImGuiTabItemFlags_NoPushId;
                }

                if (ImGui::BeginTabItem(tab.title.c_str(), &tab.open, flags)) {
                    selectedTab = i;

                    // Check for double-click to start renaming
                    if (ImGui::IsMouseDoubleClicked(0) && ImGui::IsItemHovered()) {
                        tab.renaming = true;
                    }

                    ImGui::EndTabItem();
                }

                // Handle tab closing
                if (!tab.open) {
                    CloseTab(i);
                    if (i > 0) i--;
                }
            }

            // Add a "+" button to create new tabs
            if (ImGui::TabItemButton("+", ImGuiTabItemFlags_Trailing | ImGuiTabItemFlags_NoTooltip))
            {
                AddTab("New Tab");
            }

            ImGui::EndTabBar();
        }

        // Render the content of the selected tab
        if (!tabs.empty() && selectedTab >= 0 && selectedTab < tabs.size())
        {
            ImGui::BeginChild("TabContent");
            ImGui::Text("This is tab: %s", tabs[selectedTab].title.c_str());
            ImGui::Text("You can put your tab-specific content here.");
            ImGui::EndChild();
        }
    }
    ImGui::End(); // End the dockable window
}

void TabBrowser::SaveTabs()
{
    DN_INFO("Saving tabs...");
    Project& project = GetActiveProject();
    toml::value file = toml::parse(project.GetPathAsString());
    file[guitag::TABS_KEY] = toml::array{};
    auto& tabsArray = file[guitag::TABS_KEY].as_array();
    tabsArray.clear();

    for (Tab& tab : tabs) {
        tabsArray.push_back(tab.Serialise().GetValue());
    }

    file.at(guitag::TABS_KEY).as_array_fmt().fmt = toml::array_format::multiline;
    file.at(guitag::TABS_KEY).as_array_fmt().indent_type = toml::indent_char::space;
    file.at(guitag::TABS_KEY).as_array_fmt().body_indent = 2;

    std::ofstream ofs(project.GetPathAsString());
    if (!ofs) {
        DN_FATAL("Unable to save tabs!");
    }
    
    ofs << file;  // This writes the TOML data to the file
    ofs.close();
}

void TabBrowser::LoadTabs()
{
    DN_INFO("Loading tabs...");
    Project& project = GetActiveProject();
    toml::value file = toml::parse(project.GetPathAsString());
    auto& tabsArray = file[guitag::TABS_KEY].as_array();
    tabs.clear();

    for (auto& item : tabsArray) {
        duin::TOMLValue value(item);
        Tab tab(value);
        tabs.push_back(tab);
    }
}

duin::UUID TabBrowser::GetUUID()
{
    return uuid;
}
