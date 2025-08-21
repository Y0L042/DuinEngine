#pragma once

#include "PanelFactory.h"
#include "TabBlackboard.h"

#include "PanelList.h"

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

        static std::string PanelTypeToName(PanelType type);
        static PanelType NameToPanelType(const std::string& name);

        PanelManager() = default;
        PanelManager(std::string uuidHexString);
        PanelManager(duin::JSONValue value);

        void Init();

        void SetBlackboard(std::shared_ptr<TabBlackboard> b);

        duin::JSONValue Serialise(); // Serialise panels to toml value
        void Deserialise(duin::JSONValue value); // Deserialise panels from toml value

        template<typename... Args>
        void CreatePanel(PanelType type, Args... args)
        {
            std::shared_ptr<Panel> panel;

            switch (type) {
            case PanelType::INVALID:
                panel = nullptr;
                break;
            case PanelType::DEFAULT:
                panel = std::make_shared<DefaultPanel>(std::forward<Args>(args)...);
                break;
            case PanelType::SCENETREE:
                panel = std::make_shared<SceneTreePanel>(std::forward<Args>(args)...);
                break;
            case PanelType::VIEWPORT:
                panel = std::make_shared<ViewportPanel>(std::forward<Args>(args)...);
                break;
            default:
                panel = nullptr;
                break;
            }

            if (panel!= nullptr && !panels[panel->GetUUID()]) {
                panels[panel->GetUUID()] = panel;
                if (panel->panelManager == nullptr) panel->panelManager = this;
            }
            panel->SetBlackboard(blackboard);
        }

        void DrawPanelMenu();

        void RemovePanel(const duin::UUID uuid);

        void DrawPanels();

        duin::UUID GetUUID();

    private:
        duin::UUID uuid;
        std::unordered_map<duin::UUID, std::shared_ptr<Panel>> panels;

        void ErasePanels();
};
