#include "Module_DecsWorld.h"

#include <daScript/daScript.h>
#include "../ScriptContext.h"
#include "Duin/ECS/DECS/DECS.h"
#include "decs_world.das.inc"

static void *decs_world_init_impl(das::Context *context)
{
    auto world = std::make_shared<duin::World>();
    auto *dnCtx = static_cast<duin::ScriptContext *>(context);
    void *handle = static_cast<void *>(dnCtx->scriptMemory->Add(world));

    return handle;
}

static uint64_t decs_world_create_entity_impl(void *worldHandle, const char *name)
{
    if (!worldHandle)
        return 0;
    auto *world = static_cast<duin::World *>(worldHandle);
    std::string n = name ? name : "";
    duin::Entity e = world->Entity(n);
    return e.GetID();
}

static uint64_t decs_world_create_prefab_impl(void *worldHandle, const char *name)
{
    if (!worldHandle)
        return 0;
    auto *world = static_cast<duin::World *>(worldHandle);
    std::string n = name ? name : "";
    duin::Entity e = world->Prefab(n);
    return e.GetID();
}

static void decs_world_delete_entity_impl(void *worldHandle, uint64_t entityId)
{
    if (!worldHandle || !entityId)
        return;
    auto *world = static_cast<duin::World *>(worldHandle);
    world->DeleteEntity(entityId);
}

static bool decs_world_is_alive_impl(void *worldHandle, uint64_t entityId)
{
    if (!worldHandle || !entityId)
        return false;
    auto *world = static_cast<duin::World *>(worldHandle);
    return world->IsAlive(entityId);
}

class Module_DecsWorld : public das::Module
{
  public:
    Module_DecsWorld() : das::Module("decs_world")
    {
    }

    bool initDependencies() override
    {
        das::ModuleLibrary lib(this);
        lib.addBuiltInModule();

        auto *entityMod = das::Module::require("decs_entity");
        if (!entityMod)
            DN_CORE_ERROR("decs_world: failed to find required module 'decs_entity'");
        addBuiltinDependency(lib, entityMod);

        addExtern<DAS_BIND_FUN(decs_world_init_impl)>(*this, lib, "decs_world_init_impl",
                                                      das::SideEffects::modifyExternal, "decs_world_init_impl");

        addExtern<DAS_BIND_FUN(decs_world_create_entity_impl)>(*this, lib, "decs_world_create_entity_impl",
                                                               das::SideEffects::modifyExternal,
                                                               "decs_world_create_entity_impl");
        addExtern<DAS_BIND_FUN(decs_world_create_prefab_impl)>(*this, lib, "decs_world_create_prefab_impl",
                                                               das::SideEffects::modifyExternal,
                                                               "decs_world_create_prefab_impl");
        addExtern<DAS_BIND_FUN(decs_world_delete_entity_impl)>(*this, lib, "decs_world_delete_entity_impl",
                                                               das::SideEffects::modifyExternal,
                                                               "decs_world_delete_entity_impl");
        addExtern<DAS_BIND_FUN(decs_world_is_alive_impl)>(*this, lib, "decs_world_is_alive_impl",
                                                          das::SideEffects::none, "decs_world_is_alive_impl");

        compileBuiltinModule("decs_world.das", decs_world_das, sizeof(decs_world_das));
        DN_CORE_INFO("Script Module [decs_world] initialized.");

        return true;
    }
};

REGISTER_MODULE(Module_DecsWorld);
