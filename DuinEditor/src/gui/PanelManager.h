#pragma once

#include "Panel.h"
#include "DefaultPanel.h"
#include "ViewportPanel.h"

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
            static_assert(std::is_base_of<Panel, T>::value, "T must derive from Panel");
            std::shared_ptr<T> panel = std::make_shared<T>(std::forward<Args>(args)...);
            if (!panels[panel->GetUUID()]) {
                panels[panel->GetUUID()] = panel;
            }

            return panel;
        }

        template<typename... Args>
        void CreatePanel(Panel::PanelType type, Args... args)
        {
            switch (type) {
                case Panel::INVALID:
                    break;
                case Panel::DEFAULT:
                    CreatePanel<DefaultPanel>(std::forward<Args>(args)...);
                    break;
                case Panel::VIEWPORT:
                    CreatePanel<ViewportPanel>(std::forward<Args>(args)...);
                    break;
            }
        }

        void RemovePanel(const duin::UUID uuid);
        //void RemovePanel(std::shared_ptr<Panel> panel);

        void DrawPanels();

        duin::UUID GetUUID();

    private:
        duin::UUID uuid;
        std::unordered_map<duin::UUID, std::shared_ptr<Panel>> panels;

        void ErasePanels();
};
