#include "Tab.h"
#include "GuiMeta.h"

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
    duin::TOMLValue value({
            { guitag::TAB_TITLE, title },
            { guitag::TAB_UUID, duin::UUID::ToStringHex(uuid)},
            { guitag::TAB_PANELMANAGER_UUID, GetPanelManagerID()}
        });
    
    panelManager.Serialise();

    return value;
}

void Tab::Deserialise(duin::TOMLValue value)
{
    title = value.At(guitag::TAB_TITLE).AsString();
    uuid = duin::UUID::FromStringHex(value.At(guitag::TAB_UUID).AsString());
    panelManager = PanelManager(value);
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

        ImGui::Text("This is tab: %s", title.c_str());
        ImGui::Text("Dock other panels here by setting their parent to this window.");


        // Create dockspace for the content area
        ImGuiDockNodeFlags dockspaceFlags = ImGuiDockNodeFlags_PassthruCentralNode |
            ImGuiDockNodeFlags_NoDockingInCentralNode;
        ImGuiID dockspaceID = ImGui::GetID("ContentDockspace");
        ImGui::DockSpace(dockspaceID, ImVec2(0, 0), dockspaceFlags);

        ImGui::EndChild();
    }
}

void Tab::DrawMenu()
{
    if (ImGui::BeginMenuBar()) {
        if (ImGui::BeginMenu("Panels")) {
            if (ImGui::MenuItem("Add Panel")) {
                // TODO
            }
            ImGui::EndMenu();
        }
        ImGui::EndMenuBar();
    }
}


