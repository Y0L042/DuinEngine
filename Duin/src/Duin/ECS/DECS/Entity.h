#pragma once

#include <flecs.h>
#include <functional>
#include <vector>
#include <string>
#include <type_traits>

namespace duin {
	class World;
	class Component;
	
	class Entity 
	{
	public:
		Entity();
		~Entity();

		// Operators
		bool operator==(const Entity& other) const;
		bool operator!=(const Entity& other) const;
		explicit operator bool() const;

		// Basic Properties
		World* GetWorld() const;
		bool IsValid() const;
		bool IsAlive() const;
		uint64_t GetID() const;
		std::string GetName() const;
		std::string GetPath(const std::string& sep = "::", const std::string& init_sep = "::") const;

		// Entity Management
		Entity& SetName(const std::string& name);
		Entity& Enable();
		Entity& Disable();
		Entity& Destruct();
		Entity& Clear();
		Entity Clone(bool clone_value = true) const;

		// Hierarchy Operations
		Entity& ChildOf(const Entity& parent);
		Entity& IsA(const Entity& second);
		Entity GetParent() const;
		std::vector<Entity> GetChildren() const;
		template<typename Func>
		void EachChild(Func&& func) const;

		// ========== UNIFIED COMPONENT API ==========
		
		// Check if entity has component(s)
		template <typename T>
		bool Has() const;
		
		template <typename... Components>
		bool HasAll() const;
		
		template <typename... Components>
		bool HasAny() const;

		// Add components (creates default-constructed component if it has data)
		template <typename T>
		Entity& Add();
		
		template <typename T, typename... Args>
		Entity& Add(Args&&... args); // Add with constructor args
		
		// Conditional add/remove
		template <typename T>
		Entity& AddIf(bool condition);

		// Remove components
		template <typename T>
		Entity& Remove();

		// ========== UNIFIED SET API ==========
		// This replaces all the various set() methods in flecs with a unified interface
		
		// Set component value (adds if doesn't exist)
		template <typename T>
		Entity& Set(const T& value);
		
		template <typename T>
		Entity& Set(T&& value);
		
		// Set component with constructor arguments
		template <typename T, typename... Args>
		Entity& Set(Args&&... args);
		
		// Set multiple components at once
		template <typename T1, typename T2, typename... Rest>
		Entity& Set(const T1& val1, const T2& val2, const Rest&... rest);

		// ========== UNIFIED GET API ==========
		
		// Get component (const)
		template <typename T>
		const T& Get() const;
		
		// Try get component (returns nullptr if doesn't exist)
		template <typename T>
		const T* TryGet() const;
		
		// Get mutable component
		template <typename T>
		T& GetMut();
		
		// Try get mutable component
		template <typename T>
		T* TryGetMut();
		
		// Ensure component exists (creates if needed, returns reference)
		template <typename T, typename... Args>
		T& Ensure(Args&&... args);

		// ========== UNIFIED PAIR API ==========
		// Simplifies all the pair operations
		
		template<typename First, typename Second>
		bool HasPair() const;
		
		template<typename First, typename Second>
		Entity& AddPair();
		
		template<typename First, typename Second>
		Entity& RemovePair();
		
		template<typename First, typename Second>
		Entity& SetPair(const Second& value);
		
		template<typename First, typename Second>
		Entity& SetPair(Second&& value);
		
		template<typename First, typename Second>
		const Second& GetPair() const;
		
		template<typename First, typename Second>
		const Second* TryGetPair() const;
		
		template<typename First, typename Second>
		Second& GetPairMut();
		
		// Dynamic pair operations with entity IDs
		Entity& AddPair(uint64_t first, uint64_t second);
		Entity& RemovePair(uint64_t first, uint64_t second);
		bool HasPair(uint64_t first, uint64_t second) const;

		// ========== ADVANCED OPERATIONS ==========
		
		// In-place construction (emplace)
		template<typename T, typename... Args>
		Entity& Emplace(Args&&... args);
		
		// Assign (set existing component, fails if doesn't exist)
		template<typename T>
		Entity& Assign(const T& value);
		
		template<typename T>
		Entity& Assign(T&& value);
		
		// Batch operations
		template<typename Func>
		Entity& Modify(Func&& func); // Modify multiple components in one call
		
		// Enable/Disable specific components
		template<typename T>
		Entity& EnableComponent();
		
		template<typename T>
		Entity& DisableComponent();
		
		template<typename T>
		bool IsComponentEnabled() const;

		// ========== UTILITY OPERATIONS ==========
		
		// Auto-override operations (for inheritance)
		template<typename T>
		Entity& SetAutoOverride(const T& value);
		
		template<typename T>
		Entity& SetAutoOverride(T&& value);
		
		// Scoped operations
		template<typename Func>
		Entity& WithScope(Func&& func) const;
		
		template<typename Func>
		Entity& WithComponent(Func&& func) const;
		
		// Event operations
		template<typename Event, typename... Args>
		void Emit(Args&&... args) const;
		
		// Mark component as modified (for change detection)
		template<typename T>
		void Modified() const;
		
		void Modified() const; // Mark entire entity as modified

		// ========== QUERY HELPERS ==========
		// Simplified query operations on this entity
		
		template<typename... Components, typename Func>
		void ForEachChild(Func&& func) const;
		
		template<typename Relationship, typename Func>
		void ForEachTarget(Func&& func) const;
		
		Entity GetTarget(uint64_t relationship, int32_t index = 0) const;

	private:
		friend class World;

		World *world;
		flecs::entity flecsEntity;

		Entity(const flecs::entity& entity);
		void SetWorld(World* world);
		void SetFlecsEntity(const flecs::entity& entity);

		// Internal helper for recursive Set operations
		template<typename T>
		void SetInternal(const T& value);
		
		template<typename T>
		void SetInternal(T&& value);
		
		template<typename T, typename... Rest>
		void SetMultipleInternal(const T& first, const Rest&... rest);
	};






	// ========== TEMPLATE IMPLEMENTATIONS ==========

	// Basic operations
	inline Entity::operator bool() const {
		return IsValid();
	}

	template<typename T>
	inline bool Entity::Has() const {
		return flecsEntity.has<T>();
	}

	template<typename... Components>
	inline bool Entity::HasAll() const {
		return (flecsEntity.has<Components>() && ...);
	}

	template<typename... Components>
	inline bool Entity::HasAny() const {
		return (flecsEntity.has<Components>() || ...);
	}

	// Add operations
	template<typename T>
	inline Entity& Entity::Add() {
		flecsEntity.add<T>();
		return *this;
	}

	template<typename T, typename... Args>
	inline Entity& Entity::Add(Args&&... args) {
		if constexpr (sizeof...(args) == 0) {
			flecsEntity.add<T>();
		} else {
			flecsEntity.set<T>(T{std::forward<Args>(args)...});
		}
		return *this;
	}

	template<typename T>
	inline Entity& Entity::AddIf(bool condition) {
		if (condition) {
			Add<T>();
		} else {
			Remove<T>();
		}
		return *this;
	}

	template<typename T>
	inline Entity& Entity::Remove() {
		flecsEntity.remove<T>();
		return *this;
	}

	// Unified Set API
	template<typename T>
	inline Entity& Entity::Set(const T& value) {
		flecsEntity.set<T>(value);
		return *this;
	}

	template<typename T>
	inline Entity& Entity::Set(T&& value) {
		flecsEntity.set<T>(std::forward<T>(value));
		return *this;
	}

	template<typename T, typename... Args>
	inline Entity& Entity::Set(Args&&... args) {
		if constexpr (sizeof...(args) == 1 && std::is_same_v<T, std::decay_t<std::tuple_element_t<0, std::tuple<Args...>>>>) {
			// Single argument of same type as T - use direct set
			flecsEntity.set<T>(std::forward<Args>(args)...);
		} else {
			// Multiple arguments - construct T with them
			flecsEntity.set<T>(T{std::forward<Args>(args)...});
		}
		return *this;
	}

	template<typename T1, typename T2, typename... Rest>
	inline Entity& Entity::Set(const T1& val1, const T2& val2, const Rest&... rest) {
		Set<T1>(val1);
		Set<T2>(val2);
		if constexpr (sizeof...(rest) > 0) {
			SetMultipleInternal(rest...);
		}
		return *this;
	}

	template<typename T, typename... Rest>
	inline void Entity::SetMultipleInternal(const T& first, const Rest&... rest) {
		Set<T>(first);
		if constexpr (sizeof...(rest) > 0) {
			SetMultipleInternal(rest...);
		}
	}

	// Get operations
	template<typename T>
	inline const T& Entity::Get() const {
		return flecsEntity.get<T>();
	}

	template<typename T>
	inline const T* Entity::TryGet() const {
		return flecsEntity.try_get<T>();
	}

	template<typename T>
	inline T& Entity::GetMut() {
		return flecsEntity.get_mut<T>();
	}

	template<typename T>
	inline T* Entity::TryGetMut() {
		return flecsEntity.get_mut<T>();
	}

	template<typename T, typename... Args>
	inline T& Entity::Ensure(Args&&... args) {
		return flecsEntity.ensure<T>(std::forward<Args>(args)...);
	}

	// Pair operations
	template<typename First, typename Second>
	inline bool Entity::HasPair() const {
		return flecsEntity.has<First, Second>();
	}

	template<typename First, typename Second>
	inline Entity& Entity::AddPair() {
		flecsEntity.add<First, Second>();
		return *this;
	}

	template<typename First, typename Second>
	inline Entity& Entity::RemovePair() {
		flecsEntity.remove<First, Second>();
		return *this;
	}

	template<typename First, typename Second>
	inline Entity& Entity::SetPair(const Second& value) {
		flecsEntity.set<First, Second>(value);
		return *this;
	}

	template<typename First, typename Second>
	inline Entity& Entity::SetPair(Second&& value) {
		flecsEntity.set<First, Second>(std::forward<Second>(value));
		return *this;
	}

	template<typename First, typename Second>
	inline const Second& Entity::GetPair() const {
		return flecsEntity.get<First, Second>();
	}

	template<typename First, typename Second>
	inline const Second* Entity::TryGetPair() const {
		return flecsEntity.try_get<First, Second>();
	}

	template<typename First, typename Second>
	inline Second& Entity::GetPairMut() {
		return flecsEntity.get_mut<First, Second>();
	}

	// Advanced operations
	template<typename T, typename... Args>
	inline Entity& Entity::Emplace(Args&&... args) {
		flecsEntity.emplace<T>(std::forward<Args>(args)...);
		return *this;
	}

	template<typename T>
	inline Entity& Entity::Assign(const T& value) {
		// Assign only works if component already exists
		if (!Has<T>()) {
			throw std::runtime_error("Cannot assign component that doesn't exist. Use Set() instead.");
		}
		flecsEntity.set<T>(value);
		return *this;
	}

	template<typename T>
	inline Entity& Entity::Assign(T&& value) {
		if (!Has<T>()) {
			throw std::runtime_error("Cannot assign component that doesn't exist. Use Set() instead.");
		}
		flecsEntity.set<T>(std::forward<T>(value));
		return *this;
	}

	template<typename Func>
	inline Entity& Entity::Modify(Func&& func) {
		flecsEntity.insert(std::forward<Func>(func));
		return *this;
	}

	template<typename T>
	inline Entity& Entity::EnableComponent() {
		flecsEntity.enable<T>();
		return *this;
	}

	template<typename T>
	inline Entity& Entity::DisableComponent() {
		flecsEntity.disable<T>();
		return *this;
	}

	template<typename T>
	inline bool Entity::IsComponentEnabled() const {
		return flecsEntity.enabled<T>();
	}

	// Auto-override operations
	template<typename T>
	inline Entity& Entity::SetAutoOverride(const T& value) {
		flecsEntity.set_auto_override<T>(value);
		return *this;
	}

	template<typename T>
	inline Entity& Entity::SetAutoOverride(T&& value) {
		flecsEntity.set_auto_override<T>(std::forward<T>(value));
		return *this;
	}

	// Scoped operations
	template<typename Func>
	inline Entity& Entity::WithScope(Func&& func) const {
		flecsEntity.scope(std::forward<Func>(func));
		return const_cast<Entity&>(*this);
	}

	template<typename Func>
	inline Entity& Entity::WithComponent(Func&& func) const {
		flecsEntity.with(std::forward<Func>(func));
		return const_cast<Entity&>(*this);
	}

	// Event operations
	template<typename Event, typename... Args>
	inline void Entity::Emit(Args&&... args) const {
		flecsEntity.emit<Event>(std::forward<Args>(args)...);
	}

	inline Entity& Entity::AddPair(uint64_t first, uint64_t second)
	{
		flecsEntity.add(first, second);
		return *this;
	}

	inline Entity& Entity::RemovePair(uint64_t first, uint64_t second)
	{
		flecsEntity.remove(first, second);
		return *this;
	}

	inline bool Entity::HasPair(uint64_t first, uint64_t second) const
	{
		return flecsEntity.has(first, second);
	}

	template<typename T>
	inline void Entity::Modified() const {
		flecsEntity.modified<T>();
	}

	// Hierarchy operations
	template<typename Func>
	inline void Entity::EachChild(Func&& func) const {
		flecsEntity.children([&](flecs::entity child) {
			Entity childEntity;
			childEntity.SetWorld(world);
			childEntity.SetFlecsEntity(child);
			func(childEntity);
		});
	}

	template<typename... Components, typename Func>
	inline void Entity::ForEachChild(Func&& func) const {
		flecsEntity.children([&](flecs::entity child) {
			if ((child.has<Components>() && ...)) {
				Entity childEntity;
				childEntity.SetWorld(world);
				childEntity.SetFlecsEntity(child);
				func(childEntity, child.get<Components>()...);
			}
		});
	}

	template<typename Relationship, typename Func>
	inline void Entity::ForEachTarget(Func&& func) const {
		flecsEntity.each<Relationship>([&](flecs::entity target) {
			Entity targetEntity;
			targetEntity.SetWorld(world);
			targetEntity.SetFlecsEntity(target);
			func(targetEntity);
		});
	}

	template<typename T>
	inline void Entity::SetInternal(const T& value) {
		Set<T>(value);
	}

	template<typename T>
	inline void Entity::SetInternal(T&& value) {
		Set<T>(std::forward<T>(value));
	}
}