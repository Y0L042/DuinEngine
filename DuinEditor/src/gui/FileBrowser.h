#pragma once

#include "Panel.h"
#include "FileManager.h"

#include <Duin/IO/IOModule.h>
#include <Duin/Core/Events/EventsModule.h>

#include <string>

class PanelManager;
class FileBrowser : public Panel
{
    public:

        FileBrowser(PanelManager *panelManager, const std::string& name);

        void ConnectOnFileSelected();
        void DrawContent() override;
        void DrawNode(FSNode* node, const std::string& nodeLabel);

    private:
		FSNode* selectedNode = nullptr;
        duin::Signal<const std::string&> onFileSelected;

};
