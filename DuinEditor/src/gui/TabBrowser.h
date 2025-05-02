#pragma once

#include "Tab.h"

#include <Duin/Core/Utils/UUID.h>
#include <Duin/IO/IOModule.h>

#include <vector>
#include <string>
#include <algorithm>
#include <external/imgui.h>

class TabBrowser {
public:
    TabBrowser();

    void Init();
    void Init(duin::DataValue value);

    void AddTab(const std::string& title); // Creates and saves
    void CreateTab(Tab tab); // Only creates
    void CloseTab(int index);
    void Render();
    duin::DataValue Serialise();
    void Deserialise(duin::DataValue data);

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
