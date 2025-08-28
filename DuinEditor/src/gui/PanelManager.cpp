#include "PanelManager.h"
#include "Tab.h"
#include "GuiMeta.h"

#include <Duin/Core/Application.h>

std::shared_ptr<PanelManager> PanelManager::Deserialise(Tab* owner, duin::JSONValue value)
{
    auto panelManager = std::make_shared<PanelManager>(owner);
    panelManager->Init();

    if (!value.HasMember(guitag::PANELS_ARRAY_KEY)) {
        DN_WARN("No panels array found in data: \n{}\n", value.Write());
        return panelManager;
    }
    if (!value[guitag::PANELS_ARRAY_KEY].IsArray()) {
        DN_WARN("Data not an array! \n{}\n", value.Write());
        return panelManager;
    }

    duin::JSONValue panelsArray = value[guitag::PANELS_ARRAY_KEY];
    for (auto itemJSON : panelsArray) {
        if (!itemJSON.HasMember(guitag::PANEL_TYPE)) continue;
        std::string typeStr = itemJSON[guitag::PANEL_TYPE].GetString();
        PanelType type = NameToPanelType(typeStr);
		const std::string panelName = Panel::ReadPanelName(itemJSON);
        auto panel = panelManager->CreatePanel(type, panelName);
        panel->PostDeserialise(itemJSON);
    }

    return panelManager;
}

PanelManager::PanelManager(Tab* owner)
    : owner(owner)
{
    Init();
}

void PanelManager::Init()
{
    blackboard = owner->blackboard;
}

void PanelManager::SetOwner(Tab* newOwner)
{
	owner = newOwner;
}

void PanelManager::SetBlackboard(std::shared_ptr<TabBlackboard> b)
{
    blackboard = b;
}

std::shared_ptr<TabBlackboard> PanelManager::GetBlackboard()
{
    return (blackboard != nullptr ? blackboard : nullptr);
}

duin::JSONValue PanelManager::Serialise()
{
    duin::JSONValue data;
    data.AddMember(guitag::PANELMANAGER_UUID, duin::UUID::ToStringHex(uuid));

    duin::JSONValue panelsArray;
    panelsArray.SetArray();

    int i = 0;
    for (const auto & [ uuid, panelPtr ] : panels) {
        if (panelPtr->type != PanelType::INVALID) {
            panelsArray.PushBack(panelPtr->Serialise());
        }
        ++i;
    }
    data.AddMember(guitag::PANELS_ARRAY_KEY, panelsArray);

    return data;
}

//void PanelManager::Deserialise(duin::JSONValue data)
//{
//    if (!data.HasMember(guitag::PANELS_ARRAY_KEY)) {
//        DN_WARN("No panels array found in data: \n{}\n", data.Write());
//        return;
//    }
//    if (!data[guitag::PANELS_ARRAY_KEY].IsArray()) {
//        DN_WARN("Data not an array! \n{}\n", data.Write());
//        return;
//    }
//
//    duin::JSONValue panelsArray = data[guitag::PANELS_ARRAY_KEY];
//    panels.clear();
//    for (auto itemJSON : panelsArray) {
//        if (!itemJSON.HasMember(guitag::PANEL_TYPE)) continue;
//        std::string typeStr = itemJSON[guitag::PANEL_TYPE].GetString();
//        PanelType type = NameToPanelType(typeStr);
//        auto panel = CreatePanel(type);
//		panel->Deserialise(itemJSON);
//    }
//}

std::shared_ptr<Panel> PanelManager::CreatePanel(PanelType type, const std::string& name)
{
    std::shared_ptr<Panel> panel;

    switch (type) {
    case PanelType::INVALID:
        panel = nullptr;
        break;
    case PanelType::DEFAULT:
        panel = std::make_shared<DefaultPanel>(this, name);
        break;
    case PanelType::SCENETREE:
        panel = std::make_shared<SceneTreePanel>(this, name);
        break;
    case PanelType::VIEWPORT:
        panel = std::make_shared<ViewportPanel>(this, name);
        break;
    case PanelType::FILEBROWSER:
        panel = std::make_shared<FileBrowser>(this, name);
        break;
    default:
        panel = nullptr;
        break;
    }
    
	// Add panel to map if valid and not already present
    if (panel != nullptr && !panels[panel->GetUUID()]) {
        panels[panel->GetUUID()] = panel;
        if (panel->panelManager == nullptr) panel->panelManager = this;
    }
    panel->SetBlackboard(blackboard);

    return panel;
}

void PanelManager::DrawPanelMenu()
{
    static int counter = 0;
    if (ImGui::BeginMenuBar()) {
        if (ImGui::BeginMenu("Add Panel")) {
            if (ImGui::MenuItem("Add SceneTree Panel")) {
                const std::string panelName = "SceneTree_Panel_" + std::to_string(++counter);
                auto panel = CreatePanel(PanelType::SCENETREE, panelName);
                //panel->SetPanelName(panelName);
            }
            if (ImGui::MenuItem("Add Viewport Panel")) {
                const std::string panelName = "Viewport_Panel_" + std::to_string(++counter);
                auto panel = CreatePanel(PanelType::VIEWPORT, panelName);
				//panel->SetPanelName(panelName);
            }
            if (ImGui::MenuItem("Add FileBrowser Panel")) {
                const std::string panelName = "FileBrowser_Panel_" + std::to_string(++counter);
                auto panel = CreatePanel(PanelType::FILEBROWSER, panelName);
                //panel->SetPanelName(panelName);
            }
            ImGui::EndMenu();
        }
        ImGui::EndMenuBar();
    }
}

void PanelManager::RemovePanel(const duin::UUID uuid)
{
    if (panels.contains(uuid)) {
        panels[uuid]->queuedForDeletion = true;
    }
}

void PanelManager::DrawPanels()
{
    ErasePanels();

    for (const auto& [ uuid, panelPtr ] : panels) {
        panelPtr->Draw();
    }
}

duin::UUID PanelManager::GetUUID()
{
    return uuid;
}

void PanelManager::CreatePanel_()
{
}

void PanelManager::ErasePanels()
{
    bool erased = false;
    for (auto it = panels.begin(); it != panels.end(); ) {
        if (it->second->queuedForDeletion) {
            it = panels.erase(it);
            erased = true;
        }
        else {
            it++;
        }
    }
}
