#include "FileBrowser.h"
#include "PanelManager.h"

FileBrowser::FileBrowser(PanelManager *panelManager, const std::string& name)
    : Panel(panelManager, PanelType::FILEBROWSER, name)
{
}


void FileBrowser::DrawContent()
{
    ImGui::Text("This is FileBrowser window content!");
}
