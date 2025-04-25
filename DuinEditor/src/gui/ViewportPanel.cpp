#include "ViewportPanel.h"

ViewportPanel::ViewportPanel(const std::string& name)
    : Panel(name)
{

}

void ViewportPanel::DrawContent()
{
    ImGui::Text("This is my custom window content!");
}
