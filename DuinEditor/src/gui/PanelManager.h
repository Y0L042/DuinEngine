#pragma once

//#include "PanelFactory.h"
#include "TabBlackboard.h"

#include "PanelList.h"
#include "Panel.h"
#include "DefaultPanel.h"
#include "ViewportPanel.h"
#include "SceneTreePanel.h"
#include "FileBrowser.h"

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
		static std::shared_ptr<PanelManager> Deserialise(Tab* owner, duin::JSONValue value);

        Tab* owner = nullptr;
        std::shared_ptr<TabBlackboard> blackboard = nullptr;

        PanelManager(Tab* owner);
        //PanelManager(std::string uuidHexString);
        //PanelManager(duin::JSONValue value);

        void Init();

		void SetOwner(Tab* newOwner);
        void SetBlackboard(std::shared_ptr<TabBlackboard> b);
        std::shared_ptr<TabBlackboard> GetBlackboard();

        duin::JSONValue Serialise(); // Serialise panels to toml value
        //void Deserialise(duin::JSONValue value); // Deserialise panels from toml value

		//std::string ReadPanelName(duin::JSONValue value);
        std::shared_ptr<Panel> CreatePanel(PanelType type, const std::string& name = "__NO_NAME__");
        //void CreatePanel(PanelType type, const std::string name);

        void DrawPanelMenu();

        void RemovePanel(const duin::UUID uuid);

        void DrawPanels();

        duin::UUID GetUUID();

    private:
        duin::UUID uuid;
        std::unordered_map<duin::UUID, std::shared_ptr<Panel>> panels;

        void CreatePanel_();
        void ErasePanels();
};
