#pragma once

#include "Panel.h"

#include <Duin/IO/IOModule.h>

#include <string>

class PanelManager;
class ViewportPanel : public Panel
{
    public:
        ViewportPanel(const std::string& name, PanelManager *panelManager);
        ViewportPanel(duin::DataValue value);

        void Deserialise(duin::DataValue value) override;

        void DrawContent() override;
    private:
};
