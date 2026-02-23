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

duin::Entity::Entity() : flecsEntity(flecs::entity_t(0))
{
}

duin::Entity::Entity(uint64_t id, World *world) : flecsEntity(flecs::entity(id)), world(world)
{
}

duin::Entity duin::Entity::GetTarget(uint64_t relationship, int32_t index) const
{
    return Entity(flecsEntity.target(relationship, index));
}

flecs::entity duin::Entity::GetFlecsEntity() const
{
    return flecsEntity;
}

duin::Entity::Entity(const flecs::entity &entity) : flecsEntity(entity)
{
}

duin::Entity::Entity(const flecs::entity &entity, World *world) : flecsEntity(entity), world(world)
{
}

duin::Entity::Entity(const Entity &other) : world(other.world), flecsEntity(other.flecsEntity)
{
}

duin::Entity &duin::Entity::operator=(const Entity &other)
{
    if (this != &other)
    {
        world = other.world;
        flecsEntity = other.flecsEntity;
    }
    return *this;
}

duin::Entity::Entity(Entity &&other) noexcept : world(other.world), flecsEntity(std::move(other.flecsEntity))
{
    other.world = nullptr;
}

duin::Entity &duin::Entity::operator=(Entity &&other) noexcept
{
    if (this != &other)
    {
        world = other.world;
        flecsEntity = std::move(other.flecsEntity);
        other.world = nullptr;
    }
    return *this;
}

duin::Entity::~Entity()
{
}

duin::World *duin::Entity::GetWorld() const
{
    return world;
}

bool duin::Entity::IsValid() const
{
    bool res = true && flecsEntity.is_valid() && world != nullptr;
    return res;
}

bool duin::Entity::IsAlive() const
{
    return flecsEntity.is_alive();
}

bool duin::Entity::IsTag() const
{
    bool res = ecs_id_is_tag(world->GetFlecsWorld().c_ptr(), flecsEntity.raw_id());
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
    return flecsEntity.name().c_str();
}

std::string duin::Entity::GetPath(const std::string &sep, const std::string &init_sep) const
{
    // TODO
    return std::string();
}

duin::Entity &duin::Entity::ChildOf(const Entity &parent)
{
    flecsEntity.child_of(parent.flecsEntity);
    return *this;
}

duin::Entity &duin::Entity::SetName(const std::string &name)
{
    Entity e = world->Lookup(name);
    if (!e.IsValid())
    {
        flecsEntity.set_name(name.c_str());
    }
    else
    {
        DN_CORE_WARN("Entity {} with name <{}> already exist when trying to set name for entity {}.",
                     e.flecsEntity.raw_id(), name, flecsEntity.raw_id());
    }

    return *this;
}

duin::Entity &duin::Entity::Enable()
{
    flecsEntity.enable();
    return *this;
}

duin::Entity &duin::Entity::Disable()
{
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
    flecsEntity.destruct();
    return *this;
}

duin::Entity &duin::Entity::Clear()
{
    flecsEntity.clear();
    return *this;
}

duin::Entity duin::Entity::Clone(bool clone_value) const
{
    Entity e;
    e.SetFlecsEntity(flecsEntity.clone());
    e.SetWorld(world);
    return e;
}

duin::Entity &duin::Entity::IsA(const Entity &second)
{
    flecsEntity.is_a(second.flecsEntity);
    return *this;
}

duin::Entity duin::Entity::First()
{
    Entity e;
    e.SetFlecsEntity(flecsEntity.first());
    e.SetWorld(world);

    return e;
}

duin::Entity duin::Entity::Second()
{
    Entity e;
    e.SetFlecsEntity(flecsEntity.second());
    e.SetWorld(world);

    return e;
}

duin::Entity duin::Entity::GetParent() const
{
    return Entity(flecsEntity.parent(), world);
}

std::vector<duin::Entity> duin::Entity::GetChildren() const
{
    std::vector<Entity> children;
    flecsEntity.children([&](flecs::entity child) { children.emplace_back(Entity(child, world)); });
    return children;
}

void duin::Entity::SetWorld(World *world)
{
    this->world = world;
}

void duin::Entity::SetFlecsEntity(const flecs::entity &entity)
{
    flecsEntity = entity;
}

// ========== Entity::ID Implementation ==========

duin::Entity::ID::ID() : world_(nullptr), flecsId_(static_cast<flecs::id_t>(0))
{
}

duin::Entity::ID::ID(flecs::id_t value) : world_(nullptr), flecsId_(value)
{
}

duin::Entity::ID::ID(World *world, flecs::id_t value)
    : world_(world), flecsId_(world ? world->GetFlecsWorld().c_ptr() : nullptr, value)
{
}

duin::Entity::ID::ID(World *world, flecs::id_t first, flecs::id_t second)
    : world_(world), flecsId_(world ? world->GetFlecsWorld().c_ptr() : nullptr, first, second)
{
}

duin::Entity::ID::ID(World *world, const char *expr)
    : world_(world), flecsId_(world ? world->GetFlecsWorld().c_ptr() : nullptr, expr)
{
}

duin::Entity::ID::ID(flecs::id_t first, flecs::id_t second) : world_(nullptr), flecsId_(first, second)
{
}

duin::Entity::ID::ID(const ID &first, const ID &second)
    : world_(first.world_), flecsId_(first.flecsId_, second.flecsId_)
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
    return Entity(flecsId_.entity(), world_);
}

duin::Entity::ID duin::Entity::ID::AddFlags(flecs::id_t flags) const
{
    flecs::id newId = flecsId_.add_flags(flags);
    return ID(world_, newId.raw_id());
}

duin::Entity::ID duin::Entity::ID::RemoveFlags(flecs::id_t flags) const
{
    flecs::id newId = flecsId_.remove_flags(flags);
    return ID(world_, newId.raw_id());
}

duin::Entity::ID duin::Entity::ID::RemoveFlags() const
{
    flecs::id newId = flecsId_.remove_flags();
    return ID(world_, newId.raw_id());
}

duin::Entity::ID duin::Entity::ID::RemoveGeneration() const
{
    // TODO
    flecs::entity newEntity = flecsId_.entity();
    return ID(world_, newEntity.raw_id());
}

duin::Entity duin::Entity::ID::TypeId() const
{
    return Entity(flecsId_.type_id(), world_);
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
    return Entity(flecsId_.flags(), world_);
}

bool duin::Entity::ID::HasRelation(flecs::id_t first) const
{
    return flecsId_.has_relation(first);
}

duin::Entity duin::Entity::ID::First() const
{
    return Entity(flecsId_.first(), world_);
}

duin::Entity duin::Entity::ID::Second() const
{
    return Entity(flecsId_.second(), world_);
}

std::string duin::Entity::ID::Str() const
{
    return flecsId_.str().c_str();
}

std::string duin::Entity::ID::FlagsStr() const
{
    return flecsId_.flags_str().c_str();
}

flecs::id_t duin::Entity::ID::RawId() const
{
    return flecsId_.raw_id();
}

duin::Entity::ID::operator flecs::id_t() const
{
    return flecsId_.raw_id();
}

duin::World *duin::Entity::ID::GetWorld() const
{
    return world_;
}

flecs::id duin::Entity::ID::GetFlecsId() const
{
    return flecsId_;
}
