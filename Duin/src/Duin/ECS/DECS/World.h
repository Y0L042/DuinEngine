#pragma once

#include <flecs.h>

namespace duin {
	class Entity;
	class World {
	public:
		World();
		~World();

		// Entity creation/destruction
		Entity CreateEntity(const std::string& name = "__NONAME__");
		void DeleteEntity(uint64_t id);
		void DeleteEntity(const Entity& entity);

		// Remove all entities with a given component or pair
		template<typename T>
		void DeleteWith();
		void DeleteWith(uint64_t id);
		void DeleteWith(uint64_t first, uint64_t second);

		// Remove all instances of a component or pair
		template<typename T>
		void RemoveAll();
		void RemoveAll(uint64_t id);
		void RemoveAll(uint64_t first, uint64_t second);

		// Deferred execution
		void DeferBegin();
		void DeferEnd();
		bool IsDeferred() const;
		template<typename Func>
		void Defer(Func&& func);

		// Running/progress
		bool Progress(float deltaTime = 0.0f);

		// Entity alive/valid checks
		bool IsAlive(uint64_t id) const;
		bool IsValid(uint64_t id) const;

		// Versioning
		void SetVersion(uint64_t id);

		// Utility: make entity alive (regenerate if needed)
		Entity MakeAlive(uint64_t id);

		// Utility: release world, quit
		void Release();
		void Quit();

		// Utility: iterate children of root
		template <typename Func>
		void IterateChildren(Func&& f) const;

		// Register components
		template <typename T>
		void Component();

	private:
		friend class Entity;
		flecs::world flecsWorld;

		// Prevent copying
		World(const World&) = delete;
		World& operator=(const World&) = delete;
	};

	template<typename T>
	inline void World::DeleteWith()
	{
		flecsWorld.delete_with<T>();
	}

	template<typename T>
	inline void World::RemoveAll()
	{
		flecsWorld.remove_all<T>();
	}

	template<typename Func>
	inline void World::Defer(Func&& func)
	{
		flecsWorld.defer(std::forward<Func>(func));
	}

	template<typename T>
	inline void World::Component()
	{
		flecsWorld.component<T>();
	}

	template<typename Func>
	inline void World::IterateChildren(Func&& f) const
	{
		flecsWorld.children([&](flecs::entity child) {
				Entity e;
				e.SetWorld(const_cast<World*>(this));
				e.SetFlecsEntity(child);
				f(e);
			});
	}
}