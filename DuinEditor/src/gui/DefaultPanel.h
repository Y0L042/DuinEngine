#pragma once

#include "Panel.h"

#include <string>

class PanelManager;
class DefaultPanel : public Panel
{
    public:
        DefaultPanel(const std::string& name, PanelManager *panelManager);

        void DrawContent() override;
    private:
};
