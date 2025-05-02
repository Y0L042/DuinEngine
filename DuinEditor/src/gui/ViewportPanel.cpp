#include "ViewportPanel.h"
#include "PanelManager.h"

ViewportPanel::ViewportPanel(const std::string& name, PanelManager *panelManager)
    : Panel(name, panelManager)
{
    type = Panel::VIEWPORT;
}

ViewportPanel::ViewportPanel(duin::DataValue value)
    : Panel(value)
{
    Deserialise(value);
}

void ViewportPanel::Deserialise(duin::DataValue value)
{
    DN_INFO("DOTO");
}

void ViewportPanel::DrawContent()
{
    ImGui::Text("This is my custom window content!");
}
