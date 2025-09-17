#include "dnpch.h"
#include "Entity.h"
#include "World.h"

duin::Entity::Entity()
{
}

duin::Entity duin::Entity::GetTarget(uint64_t relationship, int32_t index) const
{
    return Entity(flecsEntity.target(relationship, index));
}

duin::Entity::Entity(const flecs::entity& entity)
	: flecsEntity(entity)
{
}

duin::Entity::~Entity()
{
}

duin::World* duin::Entity::GetWorld() const
{
	return world;
}

bool duin::Entity::IsValid() const
{ 
	return flecsEntity.is_valid();
}

bool duin::Entity::IsAlive() const
{
	return flecsEntity.is_alive();
}

uint64_t duin::Entity::GetID() const
{
	return flecsEntity.id();
}

std::string duin::Entity::GetName() const
{
	return flecsEntity.name().c_str();
}

std::string duin::Entity::GetPath(const std::string& sep, const std::string& init_sep) const
{
	// TODO
	return std::string();
}

duin::Entity& duin::Entity::ChildOf(const Entity& parent)
{
	flecsEntity.child_of(parent.flecsEntity);
	return *this;
}

duin::Entity& duin::Entity::SetName(const std::string& name)
{
	flecsEntity.set_name(name.c_str());
	return *this;
}

duin::Entity& duin::Entity::Enable()
{
	flecsEntity.enable();
	return *this;
}

duin::Entity& duin::Entity::Disable()
{
	flecsEntity.disable();
	return *this;
}

bool duin::Entity::operator==(const Entity& other) const
{
	return (flecsEntity == other.flecsEntity);
}

bool duin::Entity::operator!=(const Entity& other) const
{
	return (flecsEntity != other.flecsEntity);
}

duin::Entity& duin::Entity::Destruct()
{
	flecsEntity.destruct();
	return *this;
}

duin::Entity& duin::Entity::Clear()
{
	flecsEntity.clear();
	return *this;
}

duin::Entity duin::Entity::Clone(bool clone_value) const
{
	Entity e;
	e.SetFlecsEntity(flecsEntity.clone());
	return e;
}

duin::Entity& duin::Entity::IsA(const Entity& second)
{
	flecsEntity.is_a(second.flecsEntity);
	return *this;
}

duin::Entity duin::Entity::GetParent() const
{
	return Entity(flecsEntity.parent());
}

std::vector<duin::Entity> duin::Entity::GetChildren() const
{
	std::vector<Entity> children;
	flecsEntity.children([&](flecs::entity child) { children.emplace_back(Entity(child)); });
	return children;
}



void duin::Entity::SetWorld(World* world)
{
	this->world = world;
}

void duin::Entity::SetFlecsEntity(const flecs::entity& entity)
{
	flecsEntity = entity;
}