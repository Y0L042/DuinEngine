#pragma once

#include "Duin/Core/Core.h"

#include <entt/entt.hpp>

namespace Duin
{
	class DUIN_API Registry
	{
	public:
		Registry();
		~Registry();

		entt::registry& GetENTTRegistry()
		{
			return registry;
		}

		entt::entity CreateEntity()
		{
			return registry.create();
		}

		void DestroyEntity(entt::entity& entity)
		{
			registry.destroy(entity);
		}

		template<typename Component, typename... Args>
		void AddComponent(entt::entity& entity, Args&&... args)
		{
			registry.emplace<Component>(entity, std::forward<Args>(args)...);
		}

		template<typename Component>
		Component& GetComponent(entt::entity& entity)
		{	
			return registry.get<Component>(entity);
		}

		template<typename Component>
		void RemoveComponent(entt::entity& entity)
		{
			registry.remove<Component>(entity);
		}

		template<typename Component>
		bool HasComponent(entt::entity& entity)
		{
			return registry.any_of<Component>(entity);
		}

		template<typename... Component>
		auto View()
		{
			return registry.view<Component...>();
		}

	private:
		entt::registry registry;
	};
}