#pragma once

#include <string>
#include <functional>
#include <external/imgui.h>

#include <Duin/Core/Utils/UUID.h>
#include <Duin/IO/IOModule.h>



class PanelManager;
void DrawPanelMenu(PanelManager *panelManager);

class Panel
{
    public:
        enum PanelType {
            INVALID = 0,
            DEFAULT,
            VIEWPORT
        };

        PanelType type = PanelType::INVALID;
        bool queuedForDeletion = false;

        struct MenuItem {
            std::string label;
            std::function<void()> callback;
        };

        Panel() = default;
        Panel(const std::string& name, PanelManager *panelManager);
        Panel(const std::string& name, duin::UUID uuid, PanelManager* panelManager);
        Panel(duin::DataValue value);
        virtual ~Panel() = default;


        virtual void SetupMenuBar();    // Can be overridden to customize menu
        virtual void DrawContent() = 0; // Pure virtual - must be implemented by derived classes

        virtual duin::DataValue Serialise(); // Serialise panel
        virtual void Deserialise(duin::DataValue value); // Deserialise panel

        void AddMenuItem(const std::string& menuName, const std::string& itemName, std::function<void()> callback);
        void AddSeparator(const std::string& menuName);
        duin::UUID GetUUID();
        std::string CreateUniquePanelName();
        std::pair<std::string, std::string> SplitPanelName(const std::string& panelName);
        void Draw();

    protected:
        duin::UUID uuid;
        PanelManager *panelManager;

        float barHeight = 0.0f;

    private:
        std::string panelName;
        std::string uniqueWindowName;
        bool m_isOpen;
        ImGuiWindowFlags m_windowFlags;

        // Menu storage
        std::unordered_map<std::string, std::vector<MenuItem>> m_menuItems;
        std::unordered_map<std::string, std::vector<size_t>> m_menuSeparators;

        void DrawMenu();
};
