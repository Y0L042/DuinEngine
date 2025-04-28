#pragma once

#include "Panel.h"

#include <Duin/Core/Utils/TOMLFile.h>

#include <string>

class PanelManager;
class DefaultPanel : public Panel
{
    public:
        DefaultPanel(const std::string& name, PanelManager *panelManager);
        DefaultPanel(duin::TOMLValue value);

        void Deserialise(duin::TOMLValue value) override;

        void DrawContent() override;
    private:
};
