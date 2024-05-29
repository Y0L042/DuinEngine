#pragma once

#include "Duin/Core/Core.h"
#include "Duin/Core/UUID.h"
#include "Duin/Entity/Registry.h"

#include <entt/entt.hpp>

#include <memory>
#include <unordered_map>

namespace Duin
{
	using EntitySptr = std::shared_ptr<Duin::Entity>;

	class DUIN_API Entity
	{
	public:
		static std::shared_ptr<Entity> Create(Registry* registry)
		{
			std::shared_ptr<Entity> entity = std::make_shared<Entity>(registry);
			entityStorage[entity->GetUUID()] = entity;
			return entity;
		}

		Entity(Registry* i_registry)
			: registry(i_registry)
		{
			entity = registry->CreateEntity();
		}

		~Entity()
		{
			entityStorage.erase(uuid);
		};

		Entity(const Entity&) = delete;
		Entity& operator=(const Entity&) = delete;

		UUID GetUUID() { return uuid; }

		std::shared_ptr<Entity> GetSharedPointer()
		{
			auto it = entityStorage.find(uuid);
			if (it != entityStorage.end())
			{
				return entityStorage.at(uuid);
			}
			
			return nullptr;
		}

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
		static std::unordered_map<uint64_t, std::shared_ptr<Entity>> entityStorage;

		entt::entity entity;
		Registry* registry;
		UUID uuid;
	};
}