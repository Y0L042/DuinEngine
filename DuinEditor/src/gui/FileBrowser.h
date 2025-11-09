/**
 * FileBrowser panel class
 * Displays a file browser panel in the GUI
 */

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

        void ConnectOnFileSelected(std::function<void(FSNode *)> f);
        void DrawContent() override;
        void DrawNode(FSNode* node, const std::string& nodeLabel);

    private:
		FSNode* selectedNode = nullptr;
};

struct FileObject
{
    std::string path;
};