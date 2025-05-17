#pragma once

#include "PanelFactory.h"
#include "TabBlackboard.h"
#include "Panel.h"
#include "DefaultPanel.h"
#include "ViewportPanel.h"

#include <Duin/Core/Utils/UUID.h>
#include <Duin/IO/IOModule.h>

#include <unordered_map>
#include <memory>

// Forward declaration to avoid including Singletons to avoid including EditorWindow that includes Tab that leads to circular inclusion
// TODO fix this
class Tab;
class PanelManager
{
    public:
        Tab* owner = nullptr;
        std::shared_ptr<TabBlackboard> blackboard = nullptr;

        PanelManager() = default;
        PanelManager(std::string uuidHexString);
        PanelManager(duin::DataValue value);

        void Init();

        void SetBlackboard(std::shared_ptr<TabBlackboard> b);

        duin::DataValue Serialise(); // Serialise panels to toml value
        void Deserialise(duin::DataValue value); // Deserialise panels from toml value

        template<typename... Args>
        void CreatePanel(Panel::PanelType type, Args... args)
        {
            std::shared_ptr<Panel> panel = PanelFactory::CreatePanel(type, std::forward<Args>(args)...);
            if (!panels[panel->GetUUID()]) {
                panels[panel->GetUUID()] = panel;
                if (panel->panelManager == nullptr) panel->panelManager = this;
            }
            panel->SetBlackboard(blackboard);
        }

        void RemovePanel(const duin::UUID uuid);

        void DrawPanels();

        duin::UUID GetUUID();

    private:
        duin::UUID uuid;
        std::unordered_map<duin::UUID, std::shared_ptr<Panel>> panels;

        void ErasePanels();
};
