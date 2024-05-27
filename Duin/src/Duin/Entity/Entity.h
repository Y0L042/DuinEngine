#pragma once

#include "Duin/Core/Core.h"
#include "Duin/Core/UUID.h"
#include "Duin/Entity/Registry.h"

#include <entt/entt.hpp>

namespace Duin
{
	class DUIN_API Entity
	{
	public:
		static Entity& CreateInstance()
		{
			Entity entity(Registry::GetInstancePointer());
			return entity;
		}

		static std::shared_ptr<Entity> CreatePointer()
		{
			std::shared_ptr<Entity> entitySPtr = std::make_shared<Entity>(Registry::GetInstancePointer());
			return entitySPtr;
		}

		Entity(Registry* i_registry)
			: registry(i_registry) 
		{
			entity = registry->CreateEntity();
		}
		~Entity() = default;

		UUID GetUUID() { return uuid; }

		template<typename Component, typename... Args>
		void AddComponent(Args&&... args)
		{
			registry->AddComponent<Component>(this->entity, std::forward<Args>(args)...);
		}

		template<typename Component>
		Component& GetComponent()
		{
			return registry->GetComponent<Component>(this->entity);
		}

		template<typename Component>
		void RemoveComponent()
		{
			registry->RemoveComponent<Component>(this->entity);
		}

		template<typename Component>
		bool HasComponent()
		{
			return registry->HasComponent<Component>(this->entity);
		}

	private:
		entt::entity entity;
		Registry* registry;
		UUID uuid;
	};
}