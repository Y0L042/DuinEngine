#include "PanelManager.h"
#include "GuiMeta.h"


PanelManager::PanelManager(std::string uuidHexString)
    : uuid(duin::UUID::FromStringHex(uuidHexString))
{}

PanelManager::PanelManager(duin::DataValue data)
{
    Deserialise(data);
}

void PanelManager::Init()
{
}

duin::DataValue PanelManager::Serialise()
{
    duin::DataValue data;
    data.AddMember(guitag::PANELMANAGER_UUID, duin::UUID::ToStringHex(uuid));

    duin::DataValue panelsArray;
    panelsArray.SetArray();

    int i = 0;
    for (const auto & [ uuid, panelPtr ] : panels) {
        if (panelPtr->type != Panel::INVALID) {
            panelsArray.PushBack(panelPtr->Serialise());
        }
        ++i;
    }
    data.AddMember(guitag::PANELS_ARRAY_KEY, panelsArray);

    return data;
}

void PanelManager::Deserialise(duin::DataValue data)
{
    if (!data.HasMember(guitag::PANELS_ARRAY_KEY)) {
        DN_WARN("No panels array found in data: \n{}\n", data.Write());
        return;
    }
    if (!data[guitag::PANELS_ARRAY_KEY].IsArray()) {
        DN_WARN("Data not an array! \n{}\n", data.Write());
        return;
    }

    duin::DataValue panelsArray = data[guitag::PANELS_ARRAY_KEY];
    panels.clear();
    for (auto item : panelsArray) {
        if (!item.HasMember(guitag::PANEL_TYPE)) continue;
        std::string typeStr = item[guitag::PANEL_TYPE].GetString();
        Panel::PanelType type = (Panel::PanelType)std::stoi(typeStr.c_str());
        CreatePanel(type, item);
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
    for (const auto & [ uuid, panelPtr ] : panels) {
        panelPtr->Draw();
    }

    ErasePanels();
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
    if (erased) SaveProjectConfig();
}
