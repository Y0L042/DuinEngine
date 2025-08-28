#include "DefaultPanel.h"
#include "PanelManager.h"

DefaultPanel::DefaultPanel(PanelManager *panelManager, const std::string& name)
    : Panel(panelManager, PanelType::DEFAULT, name)
{

}

void DefaultPanel::DrawContent()
{
    ImGui::Text("This is DefaultPanel window content!");
}
