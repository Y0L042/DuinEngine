#pragma once

#include "Panel.h"
#include <Duin/IO/IOModule.h>

class PanelManager;
class SceneTreePanel : public Panel
{
    public:
        SceneTreePanel(PanelManager *panelManager, const std::string& name);
        ~SceneTreePanel() override;

        void Init();

        void DrawContent() override;

    private:
};
