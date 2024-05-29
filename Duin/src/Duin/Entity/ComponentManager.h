#pragma once

#include "Duin/Core/Core.h"
#include "Duin/Entity/Registry.h"

namespace Duin
{
	struct DUIN_API ComponentManager
	{
	public:
		Registry* registry = nullptr;

		ComponentManager(Registry* registry);

		Registry* GetRegistry();

	protected:
	private:
	};
}