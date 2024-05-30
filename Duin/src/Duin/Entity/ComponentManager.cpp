#include "dnpch.h"

#include "ComponentManager.h"

namespace Duin
{
	ComponentManager::ComponentManager(Registry* registry)
		: registry(registry)
	{
	}

	Registry* ComponentManager::GetRegistry()
	{
		return registry;
	}
	void ComponentManager::SetRegistry(Registry* registry)
	{
		this->registry = registry;
	}
}