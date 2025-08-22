#pragma once

#include <Duin/ECS/ECSModule.h>
#include <Duin/Core/Events/EventsModule.h>

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

	void Clean();

	void ConnectToSignals();
	void DisconnectFromSignals();

	void Enter();
	void HandleEvents(duin::Event e);
	void Update(double delta);
	void PhysicsUpdate(double delta);
	void Draw();
	void DrawUI();
	void Exit();

private:
	duin::ECSManager ecs;

	duin::UUID onEventSignalHandle = duin::UUID::INVALID;
	duin::UUID onUpdateSignalHandle = duin::UUID::INVALID;
	duin::UUID onPhysicsUpdateSignalHandle = duin::UUID::INVALID;
	duin::UUID onDrawSignalHandle = duin::UUID::INVALID;
	duin::UUID onDrawUISignalHandle = duin::UUID::INVALID;

};