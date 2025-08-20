#include "DefaultPanel.h"
#include "PanelManager.h"

DefaultPanel::DefaultPanel(const std::string& name, PanelManager *panelManager)
    : Panel(name, panelManager)
{
    type = PanelType::DEFAULT;
}

DefaultPanel::DefaultPanel(duin::DataValue value)
    : Panel(value)
{
    Deserialise(value);
}

void DefaultPanel::Deserialise(duin::DataValue value)
{
    DN_INFO("TODO");
}

void DefaultPanel::DrawContent()
{
    ImGui::Text("This is DefaultPanel window content!");
}
