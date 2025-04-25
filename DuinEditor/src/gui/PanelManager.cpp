#include "PanelManager.h"

PanelManager::PanelManager(std::string uuidHexString)
    : uuid(duin::UUID::FromStringHex(uuidHexString))
{}

PanelManager::PanelManager(duin::TOMLValue value)
{
    Deserialise(value);
}

void PanelManager::Init()
{
}

void PanelManager::SavePanels()
{}

void PanelManager::LoadPanels()
{}

duin::TOMLValue PanelManager::Serialise()
{
    duin::TOMLValue value;
    // TODO

    return value;
}

void PanelManager::Deserialise(duin::TOMLValue value)
{
    // TODO
}

void PanelManager::RemovePanel(duin::UUID uuid)
{
    if (panels[uuid]) {
        panels.erase(uuid);
    }
}

void PanelManager::RemovePanel(std::shared_ptr<Panel> panel)
{
    RemovePanel(panel->GetUUID());
}


void PanelManager::DrawPanels()
{}

duin::UUID PanelManager::GetUUID()
{
    return uuid;
}
