#pragma once

#include "Panel.h"

#include <Duin/IO/IOModule.h>

#include <string>

class PanelManager;
class FileBrowser : public Panel
{
    public:
        FileBrowser(PanelManager *panelManager, const std::string& name);

        void DrawContent() override;
    private:
};
