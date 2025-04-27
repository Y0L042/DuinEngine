#include "ViewportPanel.h"
#include "PanelManager.h"

ViewportPanel::ViewportPanel(const std::string& name, PanelManager *panelManager)
    : Panel(name, panelManager)
{
    type = Panel::VIEWPORT;
}

void ViewportPanel::DrawContent()
{
    ImGui::Text("This is my custom window content!");
}
