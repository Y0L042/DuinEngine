#pragma once

#include "Panel.h"
#include "DefaultPanel.h"
#include "ViewportPanel.h"
#include "SceneTreePanel.h"
#include "FileBrowser.h"

#include <memory>


class PanelFactory
{
public:
	template<typename... Args>
	static std::shared_ptr<Panel> CreatePanel(PanelType type, Args... args)
	{
        std::shared_ptr<Panel> ptr;

        switch (type) {
        case Panel::INVALID:
            return nullptr;
            break;
        case Panel::DEFAULT:
            ptr = std::make_shared<DefaultPanel>(std::forward<Args>(args)...);
            break;
        case Panel::SCENETREE:
            ptr = std::make_shared<SceneTreePanel>(std::forward<Args>(args)...);
            break;
        case Panel::VIEWPORT:
            ptr = std::make_shared<ViewportPanel>(std::forward<Args>(args)...);
            break;
		case Panel::FILEBROWSER:
			ptr = std::make_shared<FileBrowser>(std::forward<Args>(args)...);
            break;
        default:
            return nullptr;
            break;
        }

        return ptr;
	}

    template<typename T, typename... Args>
    static std::shared_ptr<T> CreatePanelType(Args... args)
    {
        static_assert(std::is_base_of<Panel, T>::value, "T must derive from Panel");
        std::shared_ptr<T> panel = std::make_shared<T>(std::forward<Args>(args)...);

        return panel;
    }

private:

};