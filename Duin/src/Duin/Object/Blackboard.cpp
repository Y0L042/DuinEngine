#include "dnpch.h"

#include "Blackboard.h"

namespace Duin
{
	Blackboard::Blackboard()
	{
	}

	Blackboard::~Blackboard()
	{
	}

	void Blackboard::AddValue(std::string key, std::any value)
	{
		blackboardMap[key] = std::move(value);
	}

	std::any Blackboard::GetValue(std::string key)
	{
		auto it = blackboardMap.find(key);
		if (it != blackboardMap.end())
		{
			return it->second;
		}
		DN_CORE_WARN("Key [{}] not found in blackboard!", key);
	}

	void Blackboard::RemoveEntry(std::string key)
	{
		blackboardMap.erase(key);
	}
}