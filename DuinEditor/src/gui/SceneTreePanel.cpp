#include "SceneTreePanel.h"

SceneTreePanel::SceneTreePanel(const std::string& name, PanelManager* panelManager)
    : Panel(name, panelManager)
{
    type = PanelType::SCENETREE;
    Init();
}

SceneTreePanel::SceneTreePanel(duin::JSONValue value)
    : Panel(value)
{
    Deserialise(value);
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


