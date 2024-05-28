#include "dnpch.h"

#include "Entity.h"

namespace Duin
{
	std::unordered_map<uint64_t, std::shared_ptr<Entity>> Entity::entityStorage;
}