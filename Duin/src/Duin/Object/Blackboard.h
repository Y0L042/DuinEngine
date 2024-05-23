#pragma once

#include "Duin/Core/Core.h"
#include "Duin/Object/Object.h"

#include <unordered_map>
#include <any>
#include <string>

namespace Duin
{
	class DUIN_API Blackboard : public Object
	{
	public:
		Blackboard();
		~Blackboard();

		void AddValue(std::string key, std::any value);
		// Returns value as Any type.
		std::any GetValue(std::string key);

		// Returns value cast as Type.
		template<typename T>
		T& GetValueCast(std::string key)
		{
			return std::any_cast<T>(GetValue(key));
		}

		void RemoveEntry(std::string key);

	private:
		std::unordered_map<std::string, std::any> blackboardMap;
	};
}
