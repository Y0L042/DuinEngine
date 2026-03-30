#include "Module_DecsEntity.h"

#include <daScript/daScript.h>
#include "Duin/ECS/DECS/DECS.h"
#include "decs_entity.das.inc"

static void decs_entity_is_a_impl(void *worldHandle, uint64_t entityId, uint64_t prefabId)
{
    if (!worldHandle || !entityId)
        return;
    auto *world = static_cast<duin::World *>(worldHandle);
    duin::Entity e(entityId, world);
    duin::Entity prefab(prefabId, world);
    e.IsA(prefab);
}

static void decs_entity_add_tag_impl(void *worldHandle, uint64_t entityId, uint64_t tagId)
{
    if (!worldHandle || !entityId || !tagId)
        return;
    auto *world = static_cast<duin::World *>(worldHandle);
    duin::Entity e(entityId, world);
    e.Add(static_cast<flecs::entity_t>(tagId));
}

static bool decs_entity_has_tag_impl(void *worldHandle, uint64_t entityId, uint64_t tagId)
{
    if (!worldHandle || !entityId || !tagId)
        return false;
    auto *world = static_cast<duin::World *>(worldHandle);
    duin::Entity e(entityId, world);
    return e.Has(static_cast<flecs::entity_t>(tagId));
}

static void decs_entity_remove_tag_impl(void *worldHandle, uint64_t entityId, uint64_t tagId)
{
    if (!worldHandle || !entityId || !tagId)
        return;
    auto *world = static_cast<duin::World *>(worldHandle);
    world->GetFlecsWorld().entity(entityId).remove(static_cast<flecs::entity_t>(tagId));
}

static void decs_entity_child_of_impl(void *worldHandle, uint64_t entityId, uint64_t parentId)
{
    if (!worldHandle || !entityId || !parentId)
        return;
    auto *world = static_cast<duin::World *>(worldHandle);
    duin::Entity e(entityId, world);
    duin::Entity parent(parentId, world);
    e.ChildOf(parent);
}

static void decs_entity_set_name_impl(void *worldHandle, uint64_t entityId, const char *name)
{
    if (!worldHandle || !entityId || !name)
        return;
    auto *world = static_cast<duin::World *>(worldHandle);
    duin::Entity e(entityId, world);
    e.SetName(name);
}

static char *decs_entity_get_name_impl(void *worldHandle, uint64_t entityId, das::Context *context)
{
    if (!worldHandle || !entityId)
        return context->allocateString("", 0, nullptr);
    auto *world = static_cast<duin::World *>(worldHandle);
    duin::Entity e(entityId, world);
    std::string name = e.GetName();
    return context->allocateString(name.c_str(), (uint32_t)name.size(), nullptr);
}

static void decs_entity_enable_impl(void *worldHandle, uint64_t entityId, bool enable)
{
    if (!worldHandle || !entityId)
        return;
    auto *world = static_cast<duin::World *>(worldHandle);
    duin::Entity e(entityId, world);
    if (enable)
        e.Enable();
    else
        e.Disable();
}

static bool decs_entity_is_alive_impl(void *worldHandle, uint64_t entityId)
{
    if (!worldHandle || !entityId)
        return false;
    auto *world = static_cast<duin::World *>(worldHandle);
    return world->IsAlive(entityId);
}

static void decs_entity_destruct_impl(void *worldHandle, uint64_t entityId)
{
    if (!worldHandle || !entityId)
        return;
    auto *world = static_cast<duin::World *>(worldHandle);
    duin::Entity e(entityId, world);
    e.Destruct();
}

class Module_DecsEntity : public das::Module
{
  public:
    Module_DecsEntity() : das::Module("decs_entity")
    {
        das::ModuleLibrary lib(this);
        lib.addBuiltInModule();

        addExtern<DAS_BIND_FUN(decs_entity_is_a_impl)>(*this, lib, "decs_entity_is_a_impl",
                                                       das::SideEffects::modifyExternal, "decs_entity_is_a_impl");
        addExtern<DAS_BIND_FUN(decs_entity_add_tag_impl)>(*this, lib, "decs_entity_add_tag_impl",
                                                          das::SideEffects::modifyExternal, "decs_entity_add_tag_impl");
        addExtern<DAS_BIND_FUN(decs_entity_has_tag_impl)>(*this, lib, "decs_entity_has_tag_impl",
                                                          das::SideEffects::none, "decs_entity_has_tag_impl");
        addExtern<DAS_BIND_FUN(decs_entity_remove_tag_impl)>(
            *this, lib, "decs_entity_remove_tag_impl", das::SideEffects::modifyExternal, "decs_entity_remove_tag_impl");
        addExtern<DAS_BIND_FUN(decs_entity_child_of_impl)>(
            *this, lib, "decs_entity_child_of_impl", das::SideEffects::modifyExternal, "decs_entity_child_of_impl");
        addExtern<DAS_BIND_FUN(decs_entity_set_name_impl)>(
            *this, lib, "decs_entity_set_name_impl", das::SideEffects::modifyExternal, "decs_entity_set_name_impl");
        addExtern<DAS_BIND_FUN(decs_entity_get_name_impl)>(
            *this, lib, "decs_entity_get_name_impl", das::SideEffects::modifyExternal, "decs_entity_get_name_impl");
        addExtern<DAS_BIND_FUN(decs_entity_enable_impl)>(*this, lib, "decs_entity_enable_impl",
                                                         das::SideEffects::modifyExternal, "decs_entity_enable_impl");
        addExtern<DAS_BIND_FUN(decs_entity_is_alive_impl)>(*this, lib, "decs_entity_is_alive_impl",
                                                           das::SideEffects::none, "decs_entity_is_alive_impl");
        addExtern<DAS_BIND_FUN(decs_entity_destruct_impl)>(
            *this, lib, "decs_entity_destruct_impl", das::SideEffects::modifyExternal, "decs_entity_destruct_impl");

        compileBuiltinModule("decs_entity.das", decs_entity_das, sizeof(decs_entity_das));

        DN_CORE_INFO("Script Module [decs_entity] initialized.");
    }
};

REGISTER_MODULE(Module_DecsEntity);
