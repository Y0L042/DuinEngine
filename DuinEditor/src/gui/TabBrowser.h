#pragma once

#include "Tab.h"

#include <Duin/Core/Utils/UUID.h>

#include <vector>
#include <string>
#include <algorithm>
#include <external/imgui.h>

class TabBrowser {
public:
    TabBrowser();

    void Init();
    void AddTab(const std::string& title); // Creates and saves
    void CreateTab(Tab tab); // Only creates
    void CloseTab(int index);
    void Render();
    void SaveTabs();
    void LoadTabs();

    duin::UUID GetUUID();

private:
    duin::UUID uuid;
    std::vector<Tab> tabs;
    int selectedTab = 0;
    ImGuiID dockspaceID;

    void DrawMainTabBrowser();
    void DrawTabBar();
    void DrawTabContentArea();
    void DrawRenameTabPopup();
    bool DrawConfirmDeleteTabPopup();
};
