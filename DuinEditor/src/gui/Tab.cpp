#include "Tab.h"
#include "GuiMeta.h"

#include "DefaultPanel.h"
#include "ViewportPanel.h"

Tab::Tab(duin::TOMLValue value)
{
    Deserialise(value);
}

std::string Tab::GetPanelManagerID()
{
    return duin::UUID::ToStringHex(panelManager.GetUUID());
}

duin::UUID Tab::GetUUID()
{
    return uuid;
}

duin::TOMLValue Tab::Serialise()
{
    toml::table tabTable{
            { guitag::TAB_TITLE, title },
            { guitag::TAB_UUID, duin::UUID::ToStringHex(uuid)},
            { guitag::TAB_PANELMANAGER_UUID, GetPanelManagerID()},
            { guitag::PANELMANAGER_UUID, panelManager.Serialise().GetValue().as_table()}
    };
    toml::value value(tabTable);
    duin::TOMLValue tomlValue(value);

    return tomlValue;
}

void Tab::Deserialise(duin::TOMLValue tomlValue)
{
    toml::value& value = tomlValue.GetValue();

    if (!value.contains(guitag::TAB_TITLE)) return;
    title = value.at(guitag::TAB_TITLE).as_string();
    if (title.empty()) DN_WARN("Tab title empty when deserialising!");

    if (!value.contains(guitag::TAB_UUID)) return;
    uuid = duin::UUID::FromStringHex(value[guitag::TAB_UUID].as_string());
    if (uuid == duin::UUID::INVALID) DN_WARN("Tab uuid empty when deserialising!");

    if (!value.contains(guitag::PANELMANAGER_UUID)) return;
    duin::TOMLValue panelManagerValue(value.at(guitag::PANELMANAGER_UUID));

    panelManager = PanelManager(panelManagerValue);
}

void Tab::DrawWorkspace()
{
    /* Create the main content area below the tab bar */
    ImGuiWindowFlags workspaceFlags = ImGuiWindowFlags_NoScrollbar | 
                                      ImGuiWindowFlags_NoScrollWithMouse |
                                      ImGuiWindowFlags_NoMove |
                                      ImGuiWindowFlags_MenuBar;
    ImVec2 availableSize = ImGui::GetContentRegionAvail();

    if (ImGui::BeginChild("ContentArea", ImVec2(0, 0), 0, workspaceFlags)) {

        DrawMenu();

        // ImGui::Text("This is tab: %s", title.c_str());
        // ImGui::Text("Dock other panels here by setting their parent to this window.");


        // Create dockspace for the content area
        ImGuiDockNodeFlags dockspaceFlags = ImGuiDockNodeFlags_PassthruCentralNode;
        ImGuiID dockspaceID = ImGui::GetID("ContentDockspace");
        ImGui::DockSpace(dockspaceID, ImVec2(0, 0), dockspaceFlags);

        panelManager.DrawPanels();
    }
    ImGui::EndChild();
}

void Tab::DrawMenu()
{
    DrawPanelMenu(&panelManager);
}


