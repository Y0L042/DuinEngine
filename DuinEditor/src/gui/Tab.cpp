#include "Tab.h"
#include "GuiMeta.h"

#include "DefaultPanel.h"
#include "ViewportPanel.h"
#include <Duin/Core/Debug/DNLog.h>

std::shared_ptr<Tab> Tab::Create(EditorWindow* owner)
{
    auto tab = std::make_shared<Tab>();
    tab->SetOwner(owner);
    tab->sceneWorld = std::make_shared<SceneWorld>();
    tab->ProcessBlackboard();
    tab->CreatePanelManager();

    return tab;
}

std::shared_ptr<Tab> Tab::Create(EditorWindow* owner, duin::JSONValue value)
{
    auto tab = std::make_shared<Tab>();
    tab->SetOwner(owner);
    tab->sceneWorld = std::make_shared<SceneWorld>();
    tab->ProcessBlackboard();
    duin::JSONValue panelManagerValue = tab->Deserialise(value);
    tab->CreatePanelManager(panelManagerValue);
    return tab;
}

std::shared_ptr<Tab> Tab::Create(EditorWindow* owner, const std::string& title)
{
    auto tab = std::make_shared<Tab>();
    tab->SetOwner(owner);
    tab->sceneWorld = std::make_shared<SceneWorld>();
    tab->ProcessBlackboard();
    tab->title = title;
    tab->CreatePanelManager();

    return tab;
}

Tab::Tab(duin::JSONValue value)
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

void Tab::SetTitle(const std::string& newTitle)
{
	title = newTitle;
}

std::shared_ptr<PanelManager> Tab::CreatePanelManager()
{
    auto pm = std::make_shared<PanelManager>();
    pm->SetBlackboard(blackboard);
    this->panelManager = pm;

    return pm;
}

std::shared_ptr<PanelManager> Tab::CreatePanelManager(duin::JSONValue value)
{
    auto pm = std::make_shared<PanelManager>();
    pm->SetBlackboard(blackboard);
    pm->Deserialise(value);
    this->panelManager = pm;

    return pm;
}

std::shared_ptr<SceneWorld> Tab::GetSceneWorld()
{
    return sceneWorld;
}

void Tab::ProcessBlackboard()
{
    blackboard = std::make_shared<TabBlackboard>();
    blackboard->onFocusChange = &onFocusChange;
    blackboard->tab = this;
    blackboard->sceneWorld = sceneWorld;
}

duin::UUID Tab::GetUUID()
{
    return uuid;
}

duin::JSONValue Tab::Serialise()
{
    duin::JSONValue value;
    value.AddMember(guitag::TAB_TITLE, title);
    value.AddMember(guitag::TAB_UUID, duin::UUID::ToStringHex(uuid));
    value.AddMember(guitag::TAB_PANELMANAGER, panelManager->Serialise());

    DN_INFO("Tab: \n{}\n", value.Write());

    return value;
}

duin::JSONValue Tab::Deserialise(duin::JSONValue data)
{
    if (!data.HasMember(guitag::TAB_TITLE)) return duin::JSONValue();
    title = data[guitag::TAB_TITLE].GetString();
    if (title.empty()) DN_ERROR("Tab title empty when deserialising!");

    if (!data.HasMember(guitag::TAB_UUID)) return duin::JSONValue();
    uuid = duin::UUID::FromStringHex(data[guitag::TAB_UUID].GetString());
    if (uuid == duin::UUID::INVALID) DN_ERROR("Tab uuid invalid when deserialising!");

    if (!data.HasMember(guitag::TAB_PANELMANAGER)) {
        DN_WARN("Tab has no PanelManager!");
        return duin::JSONValue();
    }
    duin::JSONValue panelManagerValue = data[guitag::TAB_PANELMANAGER];
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


