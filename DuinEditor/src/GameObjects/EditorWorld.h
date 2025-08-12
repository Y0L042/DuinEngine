#pragma once

#include <Duin/ECS/ECSModule.h>

/**
 * @brief EditorWorld controls the game world of the Editor tab (one EditorWorld per tab).
 * It runs the ecs, and syncs data between Editor GUI, such as syncing the viewport and the scenetree panel.
 * It essentially serves as the base for the Editor GUI.
 */
class EditorWorld
{
public:
	EditorWorld();
	~EditorWorld();

	void LoadEntitiesFromFile(std::string filePath);
	void SaveEntitiesToFile(std::string filePath);

private:
	duin::ECSManager ecs;

};