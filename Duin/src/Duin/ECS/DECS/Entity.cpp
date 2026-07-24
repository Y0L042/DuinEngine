#include "dnpch.h"
#include "Entity.h"
#include "World.h"
#include "Entity_impl.hpp"
#include "Duin/Core/Debug/DNLog.h"
#include <cstdint>
#include <string>
#include <utility>
#include <vector>
#include <flecs.h>
#include <flecs/addons/cpp/c_types.hpp>
#include <flecs/addons/cpp/entity.hpp>
#include <flecs/addons/cpp/mixins/id/decl.hpp>
#include <Duin/Core/Debug/DNAssert.h>

const std::string duin::Entity::ID_DELIM = "#";

duin::Entity::Entity() : flecsEntity(flecs::entity_t(0))
{
}

duin::Entity::Entity(const flecs::world &world, uint64_t id)
    : flecsEntity(world.c_ptr(), id)
{
}

duin::Entity duin::Entity::GetTarget(uint64_t relationship, int32_t index) const
{
    if (!RequireValid())
    {
        return Entity();
    }
    return Entity(flecsEntity.target(relationship, index));
}

flecs::entity duin::Entity::GetFlecsEntity() const
{
    return flecsEntity;
}

duin::Entity::Entity(const flecs::entity &entity) : flecsEntity(entity)
{
}

duin::Entity::Entity(const Entity &other) : flecsEntity(other.flecsEntity)
{
}

duin::Entity &duin::Entity::operator=(const Entity &other)
{
    if (this != &other)
    {
        flecsEntity = other.flecsEntity;
    }
    return *this;
}

duin::Entity::Entity(Entity &&other) noexcept : flecsEntity(std::move(other.flecsEntity))
{
}

duin::Entity &duin::Entity::operator=(Entity &&other) noexcept
{
    if (this != &other)
    {
        flecsEntity = std::move(other.flecsEntity);
    }
    return *this;
}

duin::Entity::~Entity()
{
}

duin::World duin::Entity::GetWorld() const
{
    return World(flecsEntity.world().c_ptr());
}

bool duin::Entity::IsAlive() const
{
    return flecsEntity.is_alive();
}

bool duin::Entity::IsTag() const
{
    if (!RequireValid())
    {
        return false;
    }
    bool res = ecs_id_is_tag(flecsEntity.world().c_ptr(), flecsEntity.raw_id());
    return res;
}

bool duin::Entity::IsPair() const
{
    bool res = flecsEntity.is_pair();
    return res;
}

uint64_t duin::Entity::GetID() const
{
    return flecsEntity.id();
}

std::string duin::Entity::GetName() const
{
    if (!RequireValid())
    {
        return std::string();
    }
    return flecsEntity.name().c_str();
}

std::string duin::Entity::GetPath(const std::string &sep, const std::string &init_sep) const
{
    if (!RequireValid())
    {
        return std::string();
    }
    return static_cast<std::string>(flecsEntity.path());
}

duin::Entity duin::Entity::Lookup(const std::string &childName, bool searchPath)
{
    if (!RequireValid())
    {
        return Entity();
    }
    flecs::entity e = flecsEntity.lookup(childName.c_str(), searchPath);
    return Entity(e);
}

duin::Entity &duin::Entity::ChildOf(const Entity &parent)
{
    if (!RequireValid())
    {
        return *this;
    }
    flecsEntity.child_of(parent.flecsEntity);
    return *this;
}

duin::Entity &duin::Entity::SetName(const std::string &name)
{
    if (!RequireValid())
    {
        return *this;
    }
    Entity e;
    if (Parent().IsValid())
    {
        e = Parent().Lookup(name);
    }
    else
    {
        e = GetWorld().Lookup(name);
    }

    DN_CORE_ASSERT(!e.IsValid(), "No duplicate names allowed!");
    if (!e.IsValid())
    {
        flecsEntity.set_name(name.c_str());
    }
    else
    {
        DN_CORE_WARN(
            "Entity {} with name <{}> already exist when trying to set name for entity {}.",
            e.flecsEntity.raw_id(),
            name,
            flecsEntity.raw_id());
    }

    return *this;
}

duin::Entity &duin::Entity::Enable()
{
    if (!RequireValid())
    {
        return *this;
    }
    flecsEntity.enable();
    return *this;
}

duin::Entity &duin::Entity::Disable()
{
    if (!RequireValid())
    {
        return *this;
    }
    flecsEntity.disable();
    return *this;
}

bool duin::Entity::operator==(const Entity &other) const
{
    return (flecsEntity == other.flecsEntity);
}

bool duin::Entity::operator!=(const Entity &other) const
{
    return (flecsEntity != other.flecsEntity);
}

duin::Entity &duin::Entity::Destruct()
{
    if (!RequireValid())
    {
        return *this;
    }
    flecsEntity.destruct();
    return *this;
}

duin::Entity &duin::Entity::Clear()
{
    if (!RequireValid())
    {
        return *this;
    }
    flecsEntity.clear();
    return *this;
}

duin::Entity duin::Entity::Clone(bool clone_value) const
{
    if (!RequireValid())
    {
        return Entity();
    }
    return Entity(flecsEntity.clone());
}

duin::Entity &duin::Entity::IsA(const Entity &second)
{
    if (!RequireValid())
    {
        return *this;
    }
    flecsEntity.is_a(second.flecsEntity);
    return *this;
}

duin::Entity duin::Entity::First()
{
    if (!RequireValid())
    {
        return Entity();
    }
    return Entity(flecsEntity.first());
}

duin::Entity duin::Entity::Second()
{
    if (!RequireValid())
    {
        return Entity();
    }
    return Entity(flecsEntity.second());
}

duin::Entity &duin::Entity::Add(const Entity &relationship, const Entity &target)
{
    if (!RequireValid())
    {
        return *this;
    }
    flecsEntity.add(relationship.GetID(), target.GetID());

    return *this;
}

duin::Entity duin::Entity::GetParent() const
{
    if (!RequireValid())
    {
        return Entity();
    }
    return Entity(flecsEntity.parent());
}

std::vector<duin::Entity> duin::Entity::GetChildren() const
{
    if (!RequireValid())
    {
        return std::vector<Entity>();
    }
    std::vector<Entity> children;
    flecsEntity.children([&](flecs::entity child) { children.emplace_back(Entity(child)); });
    return children;
}

void duin::Entity::SetFlecsEntity(const flecs::entity &entity)
{
    flecsEntity = entity;
}

// ========== Entity::ID Implementation ==========

duin::Entity::ID::ID() : flecsId_(static_cast<flecs::id_t>(0))
{
}

duin::Entity::ID::ID(flecs::id_t value) : flecsId_(value)
{
}

duin::Entity::ID::ID(flecs::id_t first, flecs::id_t second) : flecsId_(first, second)
{
}

duin::Entity::ID::ID(const flecs::world &world, flecs::id_t value)
    : flecsId_(world.c_ptr(), value)
{
}

duin::Entity::ID::ID(const flecs::world &world, flecs::id_t first, flecs::id_t second)
    : flecsId_(world.c_ptr(), first, second)
{
}

duin::Entity::ID::ID(const flecs::world &world, const char *expr)
    : flecsId_(world.c_ptr(), expr)
{
}

duin::Entity::ID::ID(const ID &first, const ID &second)
    : flecsId_(first.flecsId_, second.flecsId_)
{
}

bool duin::Entity::ID::IsPair() const
{
    return flecsId_.is_pair();
}

bool duin::Entity::ID::IsWildcard() const
{
    return flecsId_.is_wildcard();
}

bool duin::Entity::ID::IsEntity() const
{
    return flecsId_.is_entity();
}

duin::Entity duin::Entity::ID::GetEntity() const
{
    return Entity(flecsId_.entity());
}

duin::Entity::ID duin::Entity::ID::AddFlags(flecs::id_t flags) const
{
    return ID(flecsId_.add_flags(flags));
}

duin::Entity::ID duin::Entity::ID::RemoveFlags(flecs::id_t flags) const
{
    return ID(flecsId_.remove_flags(flags));
}

duin::Entity::ID duin::Entity::ID::RemoveFlags() const
{
    return ID(flecsId_.remove_flags());
}

duin::Entity::ID duin::Entity::ID::RemoveGeneration() const
{
    // TODO
    return ID(flecsId_.entity());
}

duin::Entity duin::Entity::ID::TypeId() const
{
    return Entity(flecsId_.type_id());
}

bool duin::Entity::ID::HasFlags(flecs::id_t flags) const
{
    return flecsId_.has_flags(flags);
}

bool duin::Entity::ID::HasFlags() const
{
    return flecsId_.has_flags();
}

duin::Entity duin::Entity::ID::GetFlags() const
{
    return Entity(flecsId_.flags());
}

bool duin::Entity::ID::HasRelation(flecs::id_t first) const
{
    return flecsId_.has_relation(first);
}

duin::Entity duin::Entity::ID::First() const
{
    return Entity(flecsId_.first());
}

duin::Entity duin::Entity::ID::Second() const
{
    return Entity(flecsId_.second());
}

std::string duin::Entity::ID::Str() const
{
    return flecsId_.str().c_str();
}

std::string duin::Entity::ID::FlagsStr() const
{
    return flecsId_.flags_str().c_str();
}

uint64_t duin::Entity::ID::GetID() const
{
    return flecsId_.raw_id();
}

duin::Entity::ID::operator flecs::id_t() const
{
    return flecsId_.raw_id();
}

duin::World duin::Entity::ID::GetWorld() const
{
    return World(flecsId_.world().c_ptr());
}

flecs::id duin::Entity::ID::GetFlecsId() const
{
    return flecsId_;
}
