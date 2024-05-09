#pragma once

#include "Duin/Core/Core.h"

namespace Duin
{
	class DUIN_API ObjectManager
	{
	public:

	private:
		ObjectManager() = default;
		~ObjectManager() = default;

		static ObjectManager& GetInstance()
		{
			static ObjectManager* instance = new ObjectManager();
			return *instance;
		}
	};
}