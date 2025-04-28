#include "PanelManager.h"
#include "GuiMeta.h"


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

duin::TOMLValue PanelManager::Serialise()
{
    duin::TOMLValue tomlValue;
    toml::value& value = tomlValue.GetValue();
    value[guitag::PANELMANAGER_UUID] = duin::UUID::ToStringHex(uuid);
    value[guitag::PANELS_ARRAY_KEY] = toml::array{};
    auto& panelsArray = value[guitag::PANELS_ARRAY_KEY].as_array();
    panelsArray.clear();
    
    for (const auto & [ uuid, panelPtr ] : panels) {
        if (panelPtr->type != Panel::INVALID)
        {
            panelsArray.push_back(panelPtr->Serialise().GetValue());
        }
    }

    //value.at(guitag::PANELS_ARRAY_KEY).as_array_fmt().fmt = toml::array_format::multiline;
    //value.at(guitag::PANELS_ARRAY_KEY).as_array_fmt().indent_type = toml::indent_char::space;
    //value.at(guitag::PANELS_ARRAY_KEY).as_array_fmt().body_indent = 2;

    return tomlValue;
}

void PanelManager::Deserialise(duin::TOMLValue tomlValue)
{
    DN_INFO("Deserialising PanelManager");
    toml::value& value = tomlValue.GetValue();
    if (value.contains(guitag::PANELS_ARRAY_KEY)) {
        auto& panelsArray = value[guitag::PANELS_ARRAY_KEY].as_array();
        panels.clear();

        for (auto& item : panelsArray) {
            duin::TOMLValue itemToml(item);
            toml::value& itemValue = itemToml.GetValue();
            if (itemValue.contains(guitag::PANEL_TYPE)) {
                const char *typeStr = itemValue[guitag::PANEL_TYPE].as_string().c_str();
                Panel::PanelType type = (Panel::PanelType)atoi(typeStr);
                CreatePanel(type, itemToml);
            }
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
