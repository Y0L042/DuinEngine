#pragma once

#include "Panel.h"

#include <Duin/Core/Utils/UUID.h>
#include <Duin/Core/Utils/TOMLFile.h>

#include <unordered_map>
#include <memory>


class PanelManager
{
    public:
        PanelManager() = default;
        PanelManager(std::string uuidHexString);
        PanelManager(duin::TOMLValue value);

        void Init();
        void SavePanels();
        void LoadPanels();

        duin::TOMLValue Serialise();
        void Deserialise(duin::TOMLValue value);

		template<typename T, typename... Args>
        std::shared_ptr<T> CreatePanel(Args... args)
        {
            static_assert(std::is_base_of<Panel, T>::value, "T must derive from State");
            std::shared_ptr<T> panel = std::make_shared<T>(std::forward<Args>(args)...);
            if (!panels[panel->GetUUID()]) {
                panels[panel->GetUUID()] = panel;
            }

            return panel;
            return nullptr;
        }

        void RemovePanel(duin::UUID uuid);
        void RemovePanel(std::shared_ptr<Panel> panel);

        void DrawPanels();

        duin::UUID GetUUID();

    private:
        duin::UUID uuid;
        std::unordered_map<duin::UUID, std::shared_ptr<Panel>> panels;
};
