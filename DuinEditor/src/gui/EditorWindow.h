#pragma once

#include "Tab.h"

#include <Duin/Core/Utils/UUID.h>
#include <Duin/IO/IOModule.h>

#include <vector>
#include <string>
#include <algorithm>
#include <external/imgui.h>

class EditorWindow {
public:
    EditorWindow();

    duin::UUID GetUUID();

    void Init();
    void Init(duin::DataValue value);

    duin::DataValue Serialise();
    void Deserialise(duin::DataValue data);

    void Render();


    void CreateTab(const std::string& title);
    void AddTab(const std::string& title);
    void AddTab(duin::DataValue data);
    void CloseTab(int index);



private:
    duin::UUID uuid;
    std::vector<std::shared_ptr<Tab>> tabs;
    int selectedTab = 0;
    ImGuiID dockspaceID;

    void DrawMenu();
    void DrawMainTabBrowser();
    void DrawTabBar();
    void DrawTabContentArea();

    void DrawRenameTabPopup();
    bool DrawConfirmDeleteTabPopup();
};
