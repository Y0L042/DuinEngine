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

		static entt::entity CreateEntity()
		{
			auto& instance = GetInstance();
			return instance.registry.create();
		}

		void DestroyEntity(entt::entity& entity)
		{
			auto& instance = GetInstance();
			instance.registry.destroy(entity);
		}

		template<typename Component, typename... Args>
		void AddComponent(entt::entity& entity, Args&&... args)
		{
			auto& instance = GetInstance();
			instance.registry.emplace<Component>(entity, std::forward<Args>(args)...);
		}

		template<typename Component>
		Component& GetComponent(entt::entity& entity)
		{
			auto& instance = GetInstance();
			return instance.registry.get<Component>(entity);
		}

		template<typename Component>
		void RemoveComponent(entt::entity& entity)
		{
			auto& instance = GetInstance();
			instance.registry.remove<Component>(entity);
		}

		template<typename Component>
		bool HasComponent(entt::entity& entity)
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