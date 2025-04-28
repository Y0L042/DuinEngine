#pragma once

#include "Panel.h"

#include <Duin/Core/Utils/TOMLFile.h>

#include <string>

class PanelManager;
class ViewportPanel : public Panel
{
    public:
        ViewportPanel(const std::string& name, PanelManager *panelManager);
        ViewportPanel(duin::TOMLValue value);

        void Deserialise(duin::TOMLValue value) override;

        void DrawContent() override;
    private:
};
