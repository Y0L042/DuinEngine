#pragma once

#include "Panel.h"

#include <Duin/IO/IOModule.h>

#include <string>

class PanelManager;
class DefaultPanel : public Panel
{
    public:
        DefaultPanel(const std::string& name, PanelManager *panelManager);
        DefaultPanel(duin::DataValue value);

        void Deserialise(duin::DataValue value) override;

        void DrawContent() override;
    private:
};
