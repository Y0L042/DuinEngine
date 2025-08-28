#pragma once

#include "PanelList.h"

#include "TabBlackboard.h"

#include <string>
#include <functional>
#include <memory>
#include <external/imgui.h>

#include <Duin/Core/Utils/UUID.h>
#include <Duin/IO/IOModule.h>


class Tab;
class PanelManager;
void DrawPanelMenu(PanelManager *panelManager);

struct PanelEvent {
    enum Type {
        NONE = 0b0,
        IS_HOVERED = 0b01,
        IS_FOCUSSED = 0b10,
        TAB_FOCUSSED = 0b100
    };
    unsigned int type = Type::NONE;
    bool isHovered = false;
    bool isFocussed = false;
    bool isTabFocussed = false;
};

class Panel
{
    public:
        static std::string ReadPanelName(duin::JSONValue value);

        PanelType type = PanelType::INVALID;
        bool queuedForDeletion = false;
        std::shared_ptr<TabBlackboard> blackboard = nullptr;

        struct MenuItem {
            std::string label;
            std::function<void()> callback;
        };

		Panel(PanelManager* panelManager, PanelType type = PanelType::INVALID, const std::string& name = "INVALID_");
        virtual ~Panel() = default;

        void SetBlackboard(std::shared_ptr<TabBlackboard> b);

        virtual void SetupMenuBar();    // Can be overridden to customize menu
        virtual void DrawContent() = 0; // Pure virtual - must be implemented by derived classes
        virtual void OnPanelEvent(PanelEvent e) {};

        virtual duin::JSONValue Serialise(); // Serialise panel
        virtual void PostDeserialise(duin::JSONValue value); // Deserialise panel post creation

        void AddMenuItem(const std::string& menuName, const std::string& itemName, std::function<void()> callback);
        void AddSeparator(const std::string& menuName);
		void SetPanelName(const std::string& name);
        duin::UUID GetUUID();
        std::string CreateUniquePanelName();
        std::pair<std::string, std::string> SplitPanelName(const std::string& panelName);
        void Draw();
        void RunOnPanelEvent();

    protected:
        friend class PanelManager;

        duin::UUID uuid;
        PanelManager *panelManager = nullptr;
        Tab* ownerTab = nullptr;

        bool isHovered = false;
        bool isFocussed = false;
        bool isTabOpen = false;

        duin::UUID tabFocusSignal;

        float barHeight = 0.0f;

        std::string panelName;
        std::string uniqueWindowName;
        bool m_isOpen;
        ImGuiWindowFlags m_windowFlags;

        bool prevIsHovered = true;
        bool prevIsFocussed = true;
        bool prevIsTabOpen = true;

        // Menu storage
        std::unordered_map<std::string, std::vector<MenuItem>> m_menuItems;
        std::unordered_map<std::string, std::vector<size_t>> m_menuSeparators;

        void DrawMenu();
};
