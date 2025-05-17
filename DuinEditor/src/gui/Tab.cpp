#include "Tab.h"
#include "GuiMeta.h"

#include "DefaultPanel.h"
#include "ViewportPanel.h"
#include <Duin/Core/Debug/DNLog.h>

std::shared_ptr<Tab> Tab::Create(EditorWindow* owner)
{
    auto tab = std::make_shared<Tab>();
    tab->SetOwner(owner);
    tab->ProcessBlackboard();
    tab->CreatePanelManager();

    return tab;
}

std::shared_ptr<Tab> Tab::Create(EditorWindow* owner, duin::DataValue value)
{
    auto tab = std::make_shared<Tab>();
    tab->SetOwner(owner);
    tab->ProcessBlackboard();
    duin::DataValue pmv = tab->Deserialise(value);
    tab->CreatePanelManager(pmv);
    return tab;
}

std::shared_ptr<Tab> Tab::Create(EditorWindow* owner, const std::string& title)
{
    auto tab = std::make_shared<Tab>();
    tab->SetOwner(owner);
    tab->ProcessBlackboard();
    tab->title = title;
    tab->CreatePanelManager();

    return tab;
}

Tab::Tab(duin::DataValue value)
{
    Deserialise(value);
}

std::string Tab::GetPanelManagerID()
{
    return duin::UUID::ToStringHex(panelManager->GetUUID());
}

void Tab::SetFocussed(bool status)
{
    prevIsFocussed = isFocussed;
    isFocussed = status;
    if (prevIsFocussed != isFocussed) {
        onFocusChange.Emit(isFocussed);
    }
}

void Tab::SetOwner(EditorWindow* owner)
{
    this->owner = owner;
}

std::shared_ptr<PanelManager> Tab::CreatePanelManager()
{
    auto pm = std::make_shared<PanelManager>();
    pm->SetBlackboard(blackboard);
    this->panelManager = pm;

    return pm;
}

std::shared_ptr<PanelManager> Tab::CreatePanelManager(duin::DataValue value)
{
    auto pm = std::make_shared<PanelManager>();
    pm->SetBlackboard(blackboard);
    pm->Deserialise(value);
    this->panelManager = pm;

    return pm;
}

void Tab::ProcessBlackboard()
{
    blackboard = std::make_shared<TabBlackboard>();
    blackboard->onFocusChange = &onFocusChange;
    blackboard->tab = this;
}

duin::UUID Tab::GetUUID()
{
    return uuid;
}

duin::DataValue Tab::Serialise()
{
    duin::DataValue value;
    value.AddMember(guitag::TAB_TITLE, title);
    value.AddMember(guitag::TAB_UUID, duin::UUID::ToStringHex(uuid));
    value.AddMember(guitag::TAB_PANELMANAGER, panelManager->Serialise());

    DN_INFO("Tab: \n{}\n", value.Write());

    return value;
}

duin::DataValue Tab::Deserialise(duin::DataValue data)
{
    if (!data.HasMember(guitag::TAB_TITLE)) return duin::DataValue();
    title = data[guitag::TAB_TITLE].GetString();
    if (title.empty()) DN_ERROR("Tab title empty when deserialising!");

    if (!data.HasMember(guitag::TAB_UUID)) return duin::DataValue();
    uuid = duin::UUID::FromStringHex(data[guitag::TAB_UUID].GetString());
    if (uuid == duin::UUID::INVALID) DN_ERROR("Tab uuid invalid when deserialising!");

    if (!data.HasMember(guitag::TAB_PANELMANAGER)) {
        DN_WARN("Tab has no PanelManager!");
        return duin::DataValue();
    }
    duin::DataValue panelManagerValue = data[guitag::TAB_PANELMANAGER];
    return panelManagerValue;
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

        panelManager->DrawPanels();
    }
    ImGui::EndChild();
}

void Tab::DrawMenu()
{
    DrawPanelMenu(panelManager.get());
}


