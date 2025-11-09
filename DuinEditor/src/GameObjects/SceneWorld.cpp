#include "SceneWorld.h"
#include "EditorState_GameEditor.h"
#include "Singletons.h"
#include <fstream>
#include <Duin/IO/IOModule.h>
#include <Duin/Core/Debug/DebugModule.h>

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

std::shared_ptr<TabSignals> SceneWorld::GetTabSignalsPointer()
{
	return signals;
}

void SceneWorld::SetTabSignalsPointer(std::shared_ptr<TabSignals> ptr)
{
	signals = ptr;

	// Connect to signals
	signals->onFileSelect->Connect([&](FSNode *file){ OnFileSelect(file); });
	signals->onFileDoubleSelect->Connect([&](FSNode *file){ OnFileDoubleSelect(file); });
	signals->onFileRightSelect->Connect([&](FSNode *file){ OnFileRightSelect(file); });
}

void SceneWorld::SetActiveScene(duin::Scene newScene)
{
	activeScene = newScene;
}

duin::Scene& SceneWorld::GetActiveScene()
{
	return activeScene;
}

void SceneWorld::InstantiateActiveScene()
{
	ecs.world.from_json(activeScene.flecsJSON.c_str());
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

void SceneWorld::OnFileDoubleSelect(FSNode* file)
{
	DN_INFO("File {} double selected!", file->name);
	HandleFileDoubleSelect(file);
}

void SceneWorld::OnFileRightSelect(FSNode* file)
{
	DN_INFO("File {} right selected!", file->name);
}

void SceneWorld::HandleFileDoubleSelect(FSNode* file)
{
	switch (file->fileType)
	{
	case INVALID_EXT:
		DN_WARN("Invalid file type selected: {}", file->name);
		break;
	case IMAGE_EXT:
		DN_INFO("Image file selected: {}", file->name);
		// Handle image file logic here
		break;
	case AUDIO_EXT:
		DN_INFO("Audio file selected: {}", file->name);
		// Handle audio file logic here
		break;
	case VIDEO_EXT:
		DN_INFO("Video file selected: {}", file->name);
		// Handle video file logic here
		break;
	case MODEL_EXT:
		DN_INFO("Model file selected: {}", file->name);
		// Handle model file logic here
		break;
	case TEXT_EXT:
		DN_INFO("Text file selected: {}", file->name);
		HandleTextFileDoubleSelect(file);
		break;
	default:
		DN_WARN("Unknown file type selected: {}", file->name);
		break;
	}
}

void SceneWorld::HandleTextFileDoubleSelect(FSNode* file)
{
	switch (file->fileExt)
	{
	// Text Files
	case FILEEXT_TXT:
	case FILEEXT_JSON:
	case FILEEXT_XML:
	case FILEEXT_LUA:
	case FILEEXT_CSV:
	case FILEEXT_MD:
	case FILEEXT_INI:
	case FILEEXT_CFG:
		break;

	// Shader Files
	case FILEEXT_SHADER:
	case FILEEXT_VERT:
	case FILEEXT_FRAG:
		break;

	// Engine Files
	case FILEEXT_ECST:
		DN_INFO("Load Scene: {} ...", file->name);
		break;

	case FILEEXT_NULL:
	default:
		break;
	}
}

void SceneWorld::OnFileSelect(FSNode* file)
{
	DN_INFO("File {} selected!", file->name);
}
