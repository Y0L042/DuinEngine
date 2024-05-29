#include "dnpch.h"

#include "SceneManager.h"

namespace Duin
{
	Entity& SceneManager::CreateEntity()
	{
		return Entity::Create(registry.get());
	}
}