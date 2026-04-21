#include "Module_DnGameWorld.h"

#include <daScript/daScript.h>
#include <daScript/ast/ast.h>
#include <daScript/simulate/simulate.h>
#include "module_builtin_rtti.h"
#include "Duin/ECS/GameWorld.h"
#include "Duin/ECS/PrefabRegistry.h"
#include "Duin/Script/ScriptContext.h"
#include "Duin/Core/Maths/DuinMaths.h"

// Creates a ScriptGameWorld backed by the daslang class instance.
// Returns a void* handle stored in ScriptMemory.
static void *dn_create_gameworld_impl(void *classPtr, const das::StructInfo *info, das::Context *context)
{
    auto obj = std::make_shared<ScriptGameWorld>((char *)classPtr, info, context);
    DN_CORE_INFO("dn_create_gameworld_impl: created ScriptGameWorld for '{}'", info->name);

    auto *dnCtx = static_cast<duin::ScriptContext *>(context);
    void *handle = static_cast<void *>(dnCtx->scriptMemory->Add(obj));

    // Register as the active game world in the context so other systems can find it.
    dnCtx->gameWorld = obj.get();

    return handle;
}

// Destroys the ScriptGameWorld and unregisters it from the context.
static void *dn_gameworld_context_handle_impl(das::Context *context)
{
    auto *dnCtx = static_cast<duin::ScriptContext *>(context);
    return dnCtx->gameWorld;
}

static void dn_destroy_gameworld_impl(void *handle, das::Context *context)
{
    if (!handle)
        return;
    DN_CORE_INFO("dn_destroy_gameworld_impl: destroying ScriptGameWorld");
    auto *dnCtx = static_cast<duin::ScriptContext *>(context);
    if (dnCtx->gameWorld == static_cast<duin::GameWorld *>(handle))
        dnCtx->gameWorld = nullptr;
    dnCtx->scriptMemory->Remove(handle);
}

static uint64_t dn_gameworld_create_entity_impl(void *handle, const char *name)
{
    if (!handle)
        return 0;
    auto *gw = static_cast<duin::GameWorld *>(handle);
    std::string n = name ? name : "";
    duin::Entity e = gw->Entity(n);
    return e.GetID();
}

static void *dn_gameworld_get_flecs_world_impl(void *handle)
{
    if (!handle)
        return nullptr;
    auto *gw = static_cast<duin::GameWorld *>(handle);
    return static_cast<void *>(gw->GetFlecsWorld().c_ptr());
}

static uint64_t dn_gameworld_find_prefab_impl(void *handle, const char *name)
{
    if (!handle || !name)
        return 0;
    duin::Entity prefab = duin::PrefabRegistry::Get().Find(name);
    return prefab.GetID();
}

static void dn_gameworld_set_global_position_impl(void *handle, uint64_t entityId, float x, float y, float z)
{
    if (!handle || !entityId)
        return;
    auto *gw = static_cast<duin::GameWorld *>(handle);
    duin::Entity e(entityId, gw);
    gw->SetGlobalPosition(e, duin::Vector3{x, y, z});
}

static void dn_gameworld_get_global_position_impl(void *handle, uint64_t entityId, float *x, float *y, float *z)
{
    if (!handle || !entityId)
        return;
    auto *gw = static_cast<duin::GameWorld *>(handle);
    duin::Entity e(entityId, gw);
    duin::Vector3 pos = gw->GetGlobalPosition(e);
    *x = pos.x;
    *y = pos.y;
    *z = pos.z;
}

static void dn_gameworld_set_global_rotation_impl(void *handle, uint64_t entityId, float x, float y, float z, float w)
{
    if (!handle || !entityId)
        return;
    auto *gw = static_cast<duin::GameWorld *>(handle);
    duin::Entity e(entityId, gw);
    gw->SetGlobalRotation(e, duin::Quaternion{x, y, z, w});
}

static void dn_gameworld_get_global_rotation_impl(void *handle, uint64_t entityId, float *x, float *y, float *z,
                                                  float *w)
{
    if (!handle || !entityId)
        return;
    auto *gw = static_cast<duin::GameWorld *>(handle);
    duin::Entity e(entityId, gw);
    duin::Quaternion rot = gw->GetGlobalRotation(e);
    *x = rot.x;
    *y = rot.y;
    *z = rot.z;
    *w = rot.w;
}

static void dn_gameworld_set_global_scale_impl(void *handle, uint64_t entityId, float x, float y, float z)
{
    if (!handle || !entityId)
        return;
    auto *gw = static_cast<duin::GameWorld *>(handle);
    duin::Entity e(entityId, gw);
    gw->SetGlobalScale(e, duin::Vector3{x, y, z});
}

static void dn_gameworld_get_global_scale_impl(void *handle, uint64_t entityId, float *x, float *y, float *z)
{
    if (!handle || !entityId)
        return;
    auto *gw = static_cast<duin::GameWorld *>(handle);
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
    Module_DecsGameWorld() : das::Module("dn_gameworld_core")
    {
    }

    bool initDependencies() override
    {
        if (initialized)
            return true;

        auto *logMod = das::Module::require("dn_log");
        if (!logMod)
        {
            DN_CORE_ERROR("dn_gameworld: required module 'dn_log' not found");
            return false;
        }
        if (!logMod->initDependencies())
        {
            DN_CORE_ERROR("dn_gameworld: failed to initialize dependencies of 'dn_log'");
            return false;
        }

        auto *rttiMod = das::Module::require("rtti_core");
        if (!rttiMod)
        {
            DN_CORE_ERROR("dn_gameworld: required module 'rtti_core' not found");
            return false;
        }
        if (!rttiMod->initDependencies())
        {
            DN_CORE_ERROR("dn_gameworld: failed to initialize dependencies of 'rtti_core'");
            return false;
        }

        auto *flecsMod = das::Module::require("flecs_core");
        if (!flecsMod)
        {
            DN_CORE_ERROR("dn_gameworld: required module 'flecs_core' not found");
            return false;
        }
        if (!flecsMod->initDependencies())
        {
            DN_CORE_ERROR("dn_gameworld: failed to initialize dependencies of 'flecs_core'");
            return false;
        }

        auto *ecsMod = das::Module::require("dn_ecs_core");
        if (!ecsMod)
        {
            DN_CORE_ERROR("dn_gameworld: required module 'dn_ecs_core' not found");
            return false;
        }
        if (!ecsMod->initDependencies())
        {
            DN_CORE_ERROR("dn_gameworld: failed to initialize dependencies of 'dn_ecs_core'");
            return false;
        }

        initialized = true;

        das::ModuleLibrary lib(this);
        lib.addBuiltInModule();
        addBuiltinDependency(lib, logMod);
        addBuiltinDependency(lib, flecsMod);
        addBuiltinDependency(lib, rttiMod);
        addBuiltinDependency(lib, ecsMod);

        addExtern<DAS_BIND_FUN(dn_gameworld_context_handle_impl)>(*this, lib, "dn_gameworld_context_handle_impl",
                                                                 das::SideEffects::accessGlobal,
                                                                 "dn_gameworld_context_handle_impl")
            ->args({"context"});

        addExtern<DAS_BIND_FUN(dn_create_gameworld_impl)>(*this, lib, "dn_create_gameworld_impl",
                                                          das::SideEffects::modifyExternal, "dn_create_gameworld_impl");

        addExtern<DAS_BIND_FUN(dn_destroy_gameworld_impl)>(
            *this, lib, "dn_destroy_gameworld_impl", das::SideEffects::modifyExternal, "dn_destroy_gameworld_impl");

        addExtern<DAS_BIND_FUN(dn_gameworld_create_entity_impl)>(*this, lib, "dn_gameworld_create_entity_impl",
                                                                 das::SideEffects::modifyExternal,
                                                                 "dn_gameworld_create_entity_impl");

        addExtern<DAS_BIND_FUN(dn_gameworld_get_flecs_world_impl)>(*this, lib, "dn_gameworld_get_flecs_world_impl",
                                                                   das::SideEffects::none,
                                                                   "dn_gameworld_get_flecs_world_impl");
        addExtern<DAS_BIND_FUN(dn_gameworld_find_prefab_impl)>(*this, lib, "dn_gameworld_find_prefab_impl",
                                                               das::SideEffects::none, "dn_gameworld_find_prefab_impl");

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

        DN_CORE_INFO("Script Module [dn_gameworld_core] initialized.");

        return true;
    }
};

REGISTER_MODULE(Module_DecsGameWorld);
