#include "dnpch.h"

#include "Entity.h"

namespace Duin
{
	std::unordered_map<uint64_t, std::shared_ptr<Entity>> Entity::entityStorage;
	bool Entity::operator==(Entity& other)
	{
		return this->entity == other.entity;;
	}
}