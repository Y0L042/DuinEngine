#pragma once

#include "Duin/Core/Core.h"

#include <entt/entt.hpp>

namespace Duin
{
	class DUIN_API Registry
	{
	public:
		static Registry& GetInstance()
		{
			static Registry* instance = new Registry();
			return *instance;
		}

		static Registry* GetInstancePointer()
		{
			Registry& instance = GetInstance();
			return &instance;
		}

		static entt::registry& GetRegistry()
		{
			auto& instance = GetInstance();
			return instance.registry;
		}

		static entt::entity CreateEntity()
		{
			auto& instance = GetInstance();
			return instance.registry.create();
		}

		static void DestroyEntity(entt::entity& entity)
		{
			auto& instance = GetInstance();
			instance.registry.destroy(entity);
		}

		template<typename Component, typename... Args>
		static void AddComponent(entt::entity& entity, Args&&... args)
		{
			auto& instance = GetInstance();
			instance.registry.emplace<Component>(entity, std::forward<Args>(args)...);
		}

		template<typename Component>
		static Component& GetComponent(entt::entity& entity)
		{
			auto& instance = GetInstance();
			return instance.registry.get<Component>(entity);
		}

		template<typename Component>
		static void RemoveComponent(entt::entity& entity)
		{
			auto& instance = GetInstance();
			instance.registry.remove<Component>(entity);
		}

		template<typename Component>
		static bool HasComponent(entt::entity& entity)
		{
			auto& instance = GetInstance();
			return instance.registry.any_of<Component>(entity);
		}

	private:
		entt::registry registry;

		Registry();
		~Registry();
	};
}