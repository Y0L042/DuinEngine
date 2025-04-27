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

void PanelManager::RemovePanel(const duin::UUID uuid)
{
    if (panels[uuid]) {
        panels[uuid]->queuedForDeletion = true;
    }
}

//void PanelManager::RemovePanel(std::shared_ptr<Panel> panel)
//{
//
//    RemovePanel(panel->GetUUID());
//}

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
