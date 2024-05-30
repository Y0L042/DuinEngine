#pragma once

#include "Duin/Core/Core.h"
#include "Duin/Entity/Registry.h"

namespace Duin
{
	struct DUIN_API ComponentManager
	{
	public:
		Registry* registry = nullptr;

		ComponentManager() = default;
		ComponentManager(Registry* registry);

		Registry* GetRegistry();
		void SetRegistry(Registry* registry);

	protected:
	private:
	};
}