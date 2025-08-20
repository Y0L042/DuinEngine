#pragma once

#include <Duin/ECS/ECSModule.h>

/**
 * @brief SceneWorld controls the game world of the Editor tab (one SceneWorld per tab).
 * It runs the ecs, and syncs data between Editor GUI, such as syncing the viewport and the scenetree panel.
 * It essentially serves as the base for the Editor GUI.
 */
class SceneWorld
{
public:
	SceneWorld();
	~SceneWorld();

	void LoadEntitiesFromFile(std::string filePath);
	void SaveEntitiesToFile(std::string filePath);

private:
	duin::ECSManager ecs;

};