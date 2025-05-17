#pragma once

#include "Panel.h"
#include <Duin/IO/IOModule.h>

class PanelManager;
class SceneTreePanel : public Panel
{
    public:
        SceneTreePanel(const std::string& name, PanelManager *panelManager);
        SceneTreePanel(duin::DataValue value);
        ~SceneTreePanel() override;

        void Init();

        void DrawContent() override;

    private:
};
