#include "PanelManager.h"
#include "GuiMeta.h"

#include <Duin/Core/Application.h>

PanelManager::PanelManager(std::string uuidHexString)
    : uuid(duin::UUID::FromStringHex(uuidHexString))
{}

PanelManager::PanelManager(duin::JSONValue data)
{
    Deserialise(data);
}

void PanelManager::Init()
{

}

void PanelManager::SetBlackboard(std::shared_ptr<TabBlackboard> b)
{
    blackboard = b;
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

void PanelManager::Deserialise(duin::JSONValue data)
{
    if (!data.HasMember(guitag::PANELS_ARRAY_KEY)) {
        DN_WARN("No panels array found in data: \n{}\n", data.Write());
        return;
    }
    if (!data[guitag::PANELS_ARRAY_KEY].IsArray()) {
        DN_WARN("Data not an array! \n{}\n", data.Write());
        return;
    }

    duin::JSONValue panelsArray = data[guitag::PANELS_ARRAY_KEY];
    panels.clear();
    for (auto item : panelsArray) {
        if (!item.HasMember(guitag::PANEL_TYPE)) continue;
        std::string typeStr = item[guitag::PANEL_TYPE].GetString();
        PanelType type = PanelManager::NameToPanelType(typeStr);
        CreatePanel(type, item);
    }
}

std::string PanelManager::PanelTypeToName(PanelType type)
{
    std::string panelName = "";
    switch (type) {
    case INVALID:
        panelName = "INVALID";
        break;
    case DEFAULT:
        panelName = "DEFAULT";
        break;
    case SCENETREE:
        panelName = "SCENETREE";
        break;
    case VIEWPORT:
        panelName = "VIEWPORT";
        break;
    default:
        panelName = "__EMPTY__";
        break;  
    }

    return panelName;
}

PanelType PanelManager::NameToPanelType(const std::string& name)
{
    PanelType type = DEFAULT;
    if (name == "INVALID") type = INVALID;
    if (name == "DEFAULT") type = DEFAULT;
    if (name == "SCENETREE") type = SCENETREE;
    if (name == "VIEWPORT") type = VIEWPORT;

    return type;
}

void PanelManager::DrawPanelMenu()
{
    static int counter = 0;
    if (ImGui::BeginMenuBar()) {
        if (ImGui::BeginMenu("Add Panel")) {
            if (ImGui::MenuItem("Add SceneTree Panel")) {
                const std::string panelName = "SceneTree_Panel_" + std::to_string(++counter);
                CreatePanel(PanelType::SCENETREE, panelName, this);
            }
            if (ImGui::MenuItem("Add Viewport Panel")) {
                const std::string panelName = "Viewport_Panel_" + std::to_string(++counter);
                CreatePanel(PanelType::VIEWPORT, panelName, this);
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
