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
	static std::shared_ptr<EditorWindow> Deserialise(duin::JSONValue value);
    EditorWindow();

    duin::UUID GetUUID();

    void Init(duin::JSONValue value = duin::JSONValue::Invalid());

    duin::JSONValue Serialise();
    //void Deserialise(duin::JSONValue data);

    void Render();


    std::shared_ptr<Tab> CreateTab(const std::string& title = "__NO_TITLE__");
	void AddTab(std::shared_ptr<Tab> tab);
	void CreateAndAddTab(const std::string& title = "__NO_TITLE__");
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
