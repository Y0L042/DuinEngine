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
        if (panelPtr->type != Panel::INVALID)
        {
            panelsArray[i] = panelPtr->Serialise();
        }
        ++i;
    }
    data.AddMember(guitag::PANELS_ARRAY_KEY, panelsArray);

    return data;
}

void PanelManager::Deserialise(duin::DataValue data)
{
    if (data.HasMember(guitag::PANELS_ARRAY_KEY) && data[guitag::PANELS_ARRAY_KEY].IsArray()) {
        duin::DataValue panelsArray = data[guitag::PANELS_ARRAY_KEY];
        panels.clear();
        for (int i = 0; i < panelsArray.Capacity(); ++i) {
            if (!panelsArray[i].HasMember(guitag::PANEL_TYPE)) continue;
            std::string typeStr = panelsArray[i][guitag::PANEL_TYPE].GetString();
            Panel::PanelType type = (Panel::PanelType)atoi(typeStr.c_str());
            CreatePanel(type, panelsArray[i]);
        }
    }
}

void PanelManager::RemovePanel(const duin::UUID uuid)
{
    if (panels[uuid]) {
        panels[uuid]->queuedForDeletion = true;
    }
    SaveProjectConfig();
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
    for (auto it = panels.begin(); it != panels.end(); ) {
        if (it->second->queuedForDeletion) {
            it = panels.erase(it);
        }
        else {
            it++;
        }
    }
}
