#include "SceneWorld.h"
#include "EditorState_GameEditor.h"
#include "Singletons.h"
#include <fstream>
#include <Duin/IO/IOModule.h>

SceneWorld::SceneWorld()
{

}

SceneWorld::~SceneWorld()
{
}

void SceneWorld::LoadEntitiesFromFile(std::string filePath)
{
	std::string entities_json;
	if (duin::FileUtils::ReadFileIntoString(filePath, entities_json)) {
		ecs.world.from_json(entities_json.c_str());
	}
	else {
		DN_WARN("Could not load entities from file {} !", filePath);
	}
}

void SceneWorld::SaveEntitiesToFile(std::string filePath)
{
	std::string ecs_json = static_cast<std::string>(ecs.world.to_json());
	if (!duin::FileUtils::WriteStringIntoFile(filePath, ecs_json)) {
		DN_WARN("Could not write entities to file {} !", filePath);
	}
}

void SceneWorld::Clean()
{
	ecs.Clear();
}

void SceneWorld::ConnectToSignals()
{
	onEventSignalHandle = EditorState_GameEditor::onEventSignal.Connect([&](duin::Event e) { HandleEvents(e); });
	onUpdateSignalHandle = EditorState_GameEditor::onUpdateSignal.Connect([&](double delta) { Update(delta); });
	onPhysicsUpdateSignalHandle = EditorState_GameEditor::onPhysicsUpdateSignal.Connect([&](double delta) { PhysicsUpdate(delta); });
    onDrawSignalHandle = EditorState_GameEditor::onDrawSignal.Connect([&]() { Draw(); });
    onDrawUISignalHandle = EditorState_GameEditor::onDrawUISignal.Connect([&]() { DrawUI(); });
}

void SceneWorld::DisconnectFromSignals()
{
	EditorState_GameEditor::onEventSignal.Disconnect(onEventSignalHandle);
	EditorState_GameEditor::onPhysicsUpdateSignal.Disconnect(onPhysicsUpdateSignalHandle);
	EditorState_GameEditor::onUpdateSignal.Disconnect(onUpdateSignalHandle);
	EditorState_GameEditor::onDrawSignal.Disconnect(onDrawSignalHandle);
	EditorState_GameEditor::onDrawUISignal.Disconnect(onDrawUISignalHandle);
}

void SceneWorld::Enter()
{
	std::string projDir = GetActiveProject().GetPathAsString();
	std::string projTreeDemo = projDir + "/Demo_Scenetree.ecst";
	LoadEntitiesFromFile(projTreeDemo);
}

void SceneWorld::HandleEvents(duin::Event e)
{
}

void SceneWorld::Update(double delta)
{
}

void SceneWorld::PhysicsUpdate(double delta)
{
}

void SceneWorld::Draw()
{
}

void SceneWorld::DrawUI()
{
}

void SceneWorld::Exit()
{
}
