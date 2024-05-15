#pragma once

#include "Duin/Core/Core.h"

#include <memory>

namespace Duin
{
	class Object; // Forward Declaration
	class DUIN_API ObjectFactory
	{
	public:
		template<typename T, typename... Args>
		static std::weak_ptr<T> Instantiate(Args&&... args)
		{
			static_assert(std::is_base_of<Object, T>::value, "T must be an Object derived class");
			std::shared_ptr<T> objInstance = std::make_shared<T>(std::forward<Args>(args)...);
			std::shared_ptr<Object> objPtr = std::static_pointer_cast<Object>(objInstance);
			objPtr->Initialize();
			return objInstance;
		}
	};
}