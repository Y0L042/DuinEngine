#pragma once

#include <vector>
#include <string>
#include <algorithm>
#include <external/imgui.h>

class TabBrowser {
public:
    struct Tab {
        std::string title;
        int id;
        bool shouldClose = false;
    };

    TabBrowser();

    void AddTab(const std::string& title) {
        static int counter = 1;
        Tab newTab;
        newTab.title = title + "##" + std::to_string(counter);
        newTab.id = counter++;
        tabs.push_back(newTab);
        selectedTab = tabs.size() - 1;
    }

    void CloseTab(int index) {
        if (index < 0 || index >= tabs.size()) return;
        
        tabs.erase(tabs.begin() + index);
        if (selectedTab >= tabs.size()) {
            selectedTab = tabs.size() - 1;
        }
        if (tabs.empty()) {
            AddTab("New Tab");
        }
    }

    void Render() {
        // Tab bar at the top
        ImGui::BeginTabBar("MainTabBar", ImGuiTabBarFlags_AutoSelectNewTabs | 
                                         ImGuiTabBarFlags_Reorderable | 
                                         ImGuiTabBarFlags_FittingPolicyScroll);
        
        // Render each tab
        for (size_t i = 0; i < tabs.size(); ) {
            Tab& tab = tabs[i];
            ImGuiTabItemFlags flags = (i == selectedTab) ? ImGuiTabItemFlags_SetSelected : 0;
            
            bool tabOpen = true;
            if (ImGui::BeginTabItem(tab.title.c_str(), &tabOpen, flags)) {
                selectedTab = i;
                ImGui::EndTabItem();
            }
            
            if (!tabOpen) {
                CloseTab(i);
            } else {
                i++;
            }
        }
        
        // Add a "+" button to create new tabs
        if (ImGui::TabItemButton("+", ImGuiTabItemFlags_Trailing | ImGuiTabItemFlags_NoTooltip)) {
            AddTab("New Tab");
        }
        
        ImGui::EndTabBar();
        
        // Render the content of the selected tab
        if (!tabs.empty() && selectedTab >= 0 && selectedTab < tabs.size()) {
            ImGui::BeginChild("TabContent");
            ImGui::Text("This is tab: %s", tabs[selectedTab].title.c_str());
            ImGui::Text("You can put your tab-specific content here.");
            ImGui::EndChild();
        }
    }

private:
    std::vector<Tab> tabs;
    int selectedTab = 0;
};

// Usage example:
TabBrowser tabBrowser;

void RenderUI() {
    tabBrowser.Render();
}
