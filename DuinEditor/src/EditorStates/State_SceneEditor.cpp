#include "State_SceneEditor.h"
#include "Singleton.h"

void State_SceneEditor::Enter()
{
    CreateGUIElements();
    ProcessProject(Singleton::GetActiveProject());
}

void State_SceneEditor::OnEvent(duin::Event e)
{
}

void State_SceneEditor::Update(double delta)
{
}

void State_SceneEditor::PhysicsUpdate(double delta)
{
}

void State_SceneEditor::Draw()
{
}

void State_SceneEditor::DrawUI()
{
}

void State_SceneEditor::Exit()
{
}

void State_SceneEditor::ProcessProject(Project project)
{
    // Initialize FileManager
    fileManager.SetRootPath(project.GetPathAsString());
    fileManager.BuildFileSystemTree();
    onUpdateFileManager.Emit(&fileManager);
}

void State_SceneEditor::CreateGUIElements()
{
    sceneTabs = CreateChildObject<SceneTabs>();
    sceneTree = CreateChildObject<SceneTree>();
    sceneViewport = CreateChildObject<SceneViewport>();
    fileTree = CreateChildObject<FileTree>(this);

    fileTree->onSceneSelect.Connect([&](FSNode *sceneFile) {});
}
