#include "DefaultPanel.h"
#include "PanelManager.h"

DefaultPanel::DefaultPanel(const std::string& name, PanelManager *panelManager)
    : Panel(name, panelManager)
{
    type = Panel::DEFAULT;
}

void DefaultPanel::DrawContent()
{
    ImGui::Text("This is DefaultPanel window content!");
}
