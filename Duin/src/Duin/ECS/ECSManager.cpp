#include "dnpch.h"
#include "ECSManager.h"

namespace duin {



void ECSManager::RegisterComponents()
{
	world.component<LocalPosition2D>();
	world.component<LocalRotation2D>();
	world.component<LocalScale2D>();
	world.component<LocalVelocity2D>();

	world.component<LocalPosition3D>();
	world.component<LocalRotation3D>();
	world.component<LocalScale3D>();
	world.component<LocalVelocity3D>();
}

void ECSManager::CreatePrefabs()
{
}

}