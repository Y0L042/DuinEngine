#include "dnpch.h"
#include <daScript/daScript.h>
#include <daScript/daScriptBind.h>
#include "Duin/Scene/SceneBuilder.h"
#include "Duin/ECS/GameWorld.h"
#include "Duin/Core/Debug/DNLog.h"

static void *dn_scenebuilder_create_impl()
{
    return new duin::SceneBuilder();
}

static void dn_scenebuilder_destroy_impl(void *handle)
{
    if (handle)
        delete static_cast<duin::SceneBuilder *>(handle);
}

static void dn_packedscene_destroy_impl(void *handle)
{
    if (handle)
        delete static_cast<duin::PackedScene *>(handle);
}

static void *dn_scenebuilder_pack_world_impl(void *sbHandle, void *gwHandle)
{
    auto *sb = static_cast<duin::SceneBuilder *>(sbHandle);
    auto *gw = static_cast<duin::GameWorld *>(gwHandle);
    if (!sb || !gw)
        return nullptr;
    return new duin::PackedScene(sb->PackScene(gw));
}

static uint64_t dn_scenebuilder_instantiate_impl(void *sbHandle, void *psHandle, void *gwHandle)
{
    auto *sb = static_cast<duin::SceneBuilder *>(sbHandle);
    auto *ps = static_cast<duin::PackedScene *>(psHandle);
    auto *gw = static_cast<duin::GameWorld *>(gwHandle);
    if (!sb || !ps || !gw)
        return 0;
    duin::Entity root = sb->InstantiateScene(*ps, gw);
    return root.GetID();
}

static uint64_t dn_scenebuilder_instantiate_as_children_impl(void *sbHandle, void *psHandle, uint64_t parentId,
                                                              void *gwHandle)
{
    auto *sb = static_cast<duin::SceneBuilder *>(sbHandle);
    auto *ps = static_cast<duin::PackedScene *>(psHandle);
    auto *gw = static_cast<duin::GameWorld *>(gwHandle);
    if (!sb || !ps || !gw)
        return 0;
    duin::Entity parent = gw->MakeAlive(parentId);
    duin::Entity root = sb->InstantiateSceneAsChildren(*ps, parent);
    return root.GetID();
}

static void dn_scenebuilder_serialize_to_file_impl(void *sbHandle, void *psHandle, const char *vpath)
{
    if (!vpath || !*vpath)
        return;
    auto *sb = static_cast<duin::SceneBuilder *>(sbHandle);
    auto *ps = static_cast<duin::PackedScene *>(psHandle);
    if (!sb || !ps)
        return;
    sb->SerializeSceneToFile(*ps, vpath);
}

static void *dn_scenebuilder_deserialize_from_file_impl(void *sbHandle, const char *vpath)
{
    if (!vpath || !*vpath)
        return nullptr;
    auto *sb = static_cast<duin::SceneBuilder *>(sbHandle);
    if (!sb)
        return nullptr;
    return new duin::PackedScene(sb->DeserializeSceneFromFile(vpath));
}

class Module_DnSceneBuilder : public das::Module
{
    bool initialized = false;

  public:
    Module_DnSceneBuilder() : das::Module("dn_scenebuilder_core")
    {
    }

    bool initDependencies() override
    {
        if (initialized)
            return true;
        initialized = true;

        auto *logMod = das::Module::require("dn_log");
        if (!logMod)
            DN_CORE_ERROR("dn_scenebuilder_core: required module 'dn_log' not found");
        auto *ecsMod = das::Module::require("dn_ecs_core");
        if (!ecsMod)
            DN_CORE_ERROR("dn_scenebuilder_core: required module 'dn_ecs_core' not found");

        das::ModuleLibrary lib(this);
        lib.addBuiltInModule();
        if (logMod)
            addBuiltinDependency(lib, logMod);
        if (ecsMod)
            addBuiltinDependency(lib, ecsMod);

        das::addExtern<DAS_BIND_FUN(dn_scenebuilder_create_impl)>(
            *this, lib, "dn_scenebuilder_create", das::SideEffects::modifyExternal, "dn_scenebuilder_create_impl");
        das::addExtern<DAS_BIND_FUN(dn_scenebuilder_destroy_impl)>(
            *this, lib, "dn_scenebuilder_destroy", das::SideEffects::modifyExternal, "dn_scenebuilder_destroy_impl")
            ->args({"handle"});
        das::addExtern<DAS_BIND_FUN(dn_packedscene_destroy_impl)>(
            *this, lib, "dn_packedscene_destroy", das::SideEffects::modifyExternal, "dn_packedscene_destroy_impl")
            ->args({"handle"});
        das::addExtern<DAS_BIND_FUN(dn_scenebuilder_pack_world_impl)>(
            *this, lib, "dn_scenebuilder_pack_world", das::SideEffects::modifyExternal,
            "dn_scenebuilder_pack_world_impl")
            ->args({"handle", "world"});
        das::addExtern<DAS_BIND_FUN(dn_scenebuilder_instantiate_impl)>(
            *this, lib, "dn_scenebuilder_instantiate", das::SideEffects::modifyExternal,
            "dn_scenebuilder_instantiate_impl")
            ->args({"handle", "scene", "world"});
        das::addExtern<DAS_BIND_FUN(dn_scenebuilder_instantiate_as_children_impl)>(
            *this, lib, "dn_scenebuilder_instantiate_as_children", das::SideEffects::modifyExternal,
            "dn_scenebuilder_instantiate_as_children_impl")
            ->args({"handle", "scene", "parent", "world"});
        das::addExtern<DAS_BIND_FUN(dn_scenebuilder_serialize_to_file_impl)>(
            *this, lib, "dn_scenebuilder_serialize_to_file", das::SideEffects::modifyExternal,
            "dn_scenebuilder_serialize_to_file_impl")
            ->args({"handle", "scene", "vpath"});
        das::addExtern<DAS_BIND_FUN(dn_scenebuilder_deserialize_from_file_impl)>(
            *this, lib, "dn_scenebuilder_deserialize_from_file", das::SideEffects::modifyExternal,
            "dn_scenebuilder_deserialize_from_file_impl")
            ->args({"handle", "vpath"});

        DN_CORE_INFO("Script Module [dn_scenebuilder_core] initialized.");
        return true;
    }
};

REGISTER_MODULE(Module_DnSceneBuilder);
REGISTER_DYN_MODULE(Module_DnSceneBuilder, Module_DnSceneBuilder);
