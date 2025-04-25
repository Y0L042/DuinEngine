#pragma once

#include <string>
#include <functional>
#include <external/imgui.h>

#include <Duin/Core/Utils/UUID.h>

class Panel
{
    public:
        struct MenuItem {
            std::string label;
            std::function<void()> callback;
        };

        Panel();
        Panel(const std::string& name);
        virtual ~Panel() = default;

        void Draw();

        virtual void SetupMenuBar();    // Can be overridden to customize menu
        virtual void DrawContent() = 0; // Pure virtual - must be implemented by derived classes
        virtual void Save() {} // Save to game config
        virtual void Load() {} // Load from game config

        void AddMenuItem(const std::string& menuName, const std::string& itemName, std::function<void()> callback);
        void AddSeparator(const std::string& menuName);

        duin::UUID GetUUID();

    protected:
        duin::UUID uuid;

    private:
        std::string m_windowName;
        bool m_isOpen;
        ImGuiWindowFlags m_windowFlags;

        // Menu storage
        std::unordered_map<std::string, std::vector<MenuItem>> m_menuItems;
        std::unordered_map<std::string, std::vector<size_t>> m_menuSeparators;
};
