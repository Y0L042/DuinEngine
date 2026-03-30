#include "Module_DnGameWorld.h"

#include <daScript/daScript.h>
#include "Duin/ECS/GameWorld.h"
#include "Duin/ECS/PrefabRegistry.h"
#include "Duin/Script/ScriptContext.h"
#include "Duin/Core/Maths/DuinMaths.h"
#include "dn_gameworld.das.inc"

static void *decs_get_gameworld_impl(das::Context *context)
{
    auto *dnCtx = static_cast<duin::ScriptContext *>(context);
    return dnCtx ? static_cast<void *>(dnCtx->gameWorld) : nullptr;
}

static uint64_t dn_gameworld_find_prefab_impl(void *worldHandle, const char *name)
{
    if (!worldHandle || !name)
        return 0;
    duin::Entity prefab = duin::PrefabRegistry::Get().Find(name);
    return prefab.GetID();
}

static void dn_gameworld_set_global_position_impl(void *worldHandle, uint64_t entityId, float x, float y, float z)
{
    if (!worldHandle || !entityId)
        return;
    auto *gw = static_cast<duin::GameWorld *>(worldHandle);
    duin::Entity e(entityId, gw);
    gw->SetGlobalPosition(e, duin::Vector3{x, y, z});
}

static void dn_gameworld_get_global_position_impl(void *worldHandle, uint64_t entityId, float *x, float *y, float *z)
{
    if (!worldHandle || !entityId)
        return;
    auto *gw = static_cast<duin::GameWorld *>(worldHandle);
    duin::Entity e(entityId, gw);
    duin::Vector3 pos = gw->GetGlobalPosition(e);
    *x = pos.x;
    *y = pos.y;
    *z = pos.z;
}

static void dn_gameworld_set_global_rotation_impl(void *worldHandle, uint64_t entityId, float x, float y, float z,
                                                     float w)
{
    if (!worldHandle || !entityId)
        return;
    auto *gw = static_cast<duin::GameWorld *>(worldHandle);
    duin::Entity e(entityId, gw);
    gw->SetGlobalRotation(e, duin::Quaternion{x, y, z, w});
}

static void dn_gameworld_get_global_rotation_impl(void *worldHandle, uint64_t entityId, float *x, float *y, float *z,
                                                     float *w)
{
    if (!worldHandle || !entityId)
        return;
    auto *gw = static_cast<duin::GameWorld *>(worldHandle);
    duin::Entity e(entityId, gw);
    duin::Quaternion rot = gw->GetGlobalRotation(e);
    *x = rot.x;
    *y = rot.y;
    *z = rot.z;
    *w = rot.w;
}

static void dn_gameworld_set_global_scale_impl(void *worldHandle, uint64_t entityId, float x, float y, float z)
{
    if (!worldHandle || !entityId)
        return;
    auto *gw = static_cast<duin::GameWorld *>(worldHandle);
    duin::Entity e(entityId, gw);
    gw->SetGlobalScale(e, duin::Vector3{x, y, z});
}

static void dn_gameworld_get_global_scale_impl(void *worldHandle, uint64_t entityId, float *x, float *y, float *z)
{
    if (!worldHandle || !entityId)
        return;
    auto *gw = static_cast<duin::GameWorld *>(worldHandle);
    duin::Entity e(entityId, gw);
    duin::Vector3 scale = gw->GetGlobalScale(e);
    *x = scale.x;
    *y = scale.y;
    *z = scale.z;
}

class Module_DecsGameWorld : public das::Module
{
    bool initialized = false;

  public:
    Module_DecsGameWorld() : das::Module("dn_gameworld") {}

    bool initDependencies() override
    {
        if (initialized)
            return true;

        initialized = true;

        das::ModuleLibrary lib(this);
        lib.addBuiltInModule();
        auto *logMod = das::Module::require("dn_log");
        if (!logMod)
            DN_CORE_ERROR("dn_gameworld: failed to find required module 'dn_log'");
        addBuiltinDependency(lib, logMod);
        auto *worldMod = das::Module::require("decs_world");
        if (!worldMod)
            DN_CORE_ERROR("dn_gameworld: failed to find required module 'decs_world'");
        addBuiltinDependency(lib, worldMod);
        auto *entityMod = das::Module::require("decs_entity");
        if (!entityMod)
            DN_CORE_ERROR("dn_gameworld: failed to find required module 'decs_entity'");
        addBuiltinDependency(lib, entityMod);

        addExtern<DAS_BIND_FUN(decs_get_gameworld_impl)>(*this, lib, "decs_get_gameworld_impl",
                                                          das::SideEffects::modifyExternal, "decs_get_gameworld_impl");
        addExtern<DAS_BIND_FUN(dn_gameworld_find_prefab_impl)>(*this, lib, "dn_gameworld_find_prefab_impl",
                                                                  das::SideEffects::none,
                                                                  "dn_gameworld_find_prefab_impl");
        addExtern<DAS_BIND_FUN(dn_gameworld_set_global_position_impl)>(
            *this, lib, "dn_gameworld_set_global_position_impl", das::SideEffects::modifyExternal,
            "dn_gameworld_set_global_position_impl");
        addExtern<DAS_BIND_FUN(dn_gameworld_get_global_position_impl)>(
            *this, lib, "dn_gameworld_get_global_position_impl", das::SideEffects::modifyExternal,
            "dn_gameworld_get_global_position_impl");
        addExtern<DAS_BIND_FUN(dn_gameworld_set_global_rotation_impl)>(
            *this, lib, "dn_gameworld_set_global_rotation_impl", das::SideEffects::modifyExternal,
            "dn_gameworld_set_global_rotation_impl");
        addExtern<DAS_BIND_FUN(dn_gameworld_get_global_rotation_impl)>(
            *this, lib, "dn_gameworld_get_global_rotation_impl", das::SideEffects::modifyExternal,
            "dn_gameworld_get_global_rotation_impl");
        addExtern<DAS_BIND_FUN(dn_gameworld_set_global_scale_impl)>(*this, lib, "dn_gameworld_set_global_scale_impl",
                                                                       das::SideEffects::modifyExternal,
                                                                       "dn_gameworld_set_global_scale_impl");
        addExtern<DAS_BIND_FUN(dn_gameworld_get_global_scale_impl)>(*this, lib, "dn_gameworld_get_global_scale_impl",
                                                                       das::SideEffects::modifyExternal,
                                                                       "dn_gameworld_get_global_scale_impl");

        compileBuiltinModule("dn_gameworld.das", dn_gameworld_das, sizeof(dn_gameworld_das));

        DN_CORE_INFO("Script Module [dn_gameworld] initialized.");

        return true;
    }
};

REGISTER_MODULE(Module_DecsGameWorld);
