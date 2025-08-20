#include "EditorWorld.h"
#include <fstream>
#include <Duin/IO/IOModule.h>

EditorWorld::EditorWorld()
{

}

EditorWorld::~EditorWorld()
{
}

void EditorWorld::LoadEntitiesFromFile(std::string filePath)
{
	std::string entities_json;
	if (duin::FileUtils::ReadFileIntoString(filePath, entities_json)) {
		ecs.world.from_json(entities_json.c_str());
	}
	else {
		DN_WARN("Could not load entities from file {} !", filePath);
	}
}

void EditorWorld::SaveEntitiesToFile(std::string filePath)
{
	std::string ecs_json = static_cast<std::string>(ecs.world.to_json());
	if (!duin::FileUtils::WriteStringIntoFile(filePath, ecs_json)) {
		DN_WARN("Could not write entities to file {} !", filePath);
	}
}
