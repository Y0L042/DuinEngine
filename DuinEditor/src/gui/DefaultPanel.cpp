#include "DefaultPanel.h"
#include "PanelManager.h"

DefaultPanel::DefaultPanel(const std::string& name, PanelManager *panelManager)
    : Panel(name, panelManager)
{
    type = Panel::DEFAULT;
}

DefaultPanel::DefaultPanel(duin::TOMLValue value)
    : Panel(value)
{
    Deserialise(value);
}

void DefaultPanel::Deserialise(duin::TOMLValue value)
{
    DN_INFO("TODO");
}

void DefaultPanel::DrawContent()
{
    ImGui::Text("This is DefaultPanel window content!");
}
