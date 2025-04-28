#pragma once

#include "PanelFactory.h"
#include "Panel.h"
#include "DefaultPanel.h"
#include "ViewportPanel.h"

#include <Duin/Core/Utils/UUID.h>
#include <Duin/Core/Utils/TOMLFile.h>

#include <unordered_map>
#include <memory>

// Forward declaration to avoid including Singletons to avoid including TabBrowser that includes Tab that leads to circular inclusion
// TODO fix this
void SaveProjectConfig();

class PanelManager
{
    public:
        PanelManager() = default;
        PanelManager(std::string uuidHexString);
        PanelManager(duin::TOMLValue value);

        void Init();

        duin::TOMLValue Serialise(); // Serialise panels to toml value
        void Deserialise(duin::TOMLValue value); // Deserialise panels from toml value

        template<typename... Args>
        void CreatePanel(Panel::PanelType type, Args... args)
        {
            std::shared_ptr<Panel> panel = PanelFactory::CreatePanel(type, std::forward<Args>(args)...);
            if (!panels[panel->GetUUID()]) {
                panels[panel->GetUUID()] = panel;
            }
            SaveProjectConfig();
        }

        void RemovePanel(const duin::UUID uuid);

        void DrawPanels();

        duin::UUID GetUUID();

    private:
        duin::UUID uuid;
        std::unordered_map<duin::UUID, std::shared_ptr<Panel>> panels;

        void ErasePanels();
};
