#include "dnpch.h"

#include "SceneManager.h"

namespace Duin
{
	std::shared_ptr<Entity> SceneManager::CreateEntity()
	{
		return Entity::Create(registry.get());
	}
}