#include "SceneTreePanel.h"

SceneTreePanel::SceneTreePanel(PanelManager* panelManager, const std::string& name)
    : Panel(panelManager, PanelType::SCENETREE, name)
{
    Init();
}

SceneTreePanel::~SceneTreePanel()
{
}

void SceneTreePanel::Init()
{
}

void SceneTreePanel::DrawContent()
{
    ImGui::Text("This is SceneTree window content!");
}


