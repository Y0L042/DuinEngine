#include "Module_DnECS.h"

#include <daScript/daScript.h>
#include <daScript/daScriptBind.h>
#include "Duin/ECS/DECS/DECS.h"
#include "Duin/ECS/ECSComponents.h"
#include "Duin/ECS/ECSTag.h"
#include "Duin/ECS/GameWorld.h"
#include "Duin/ECS/PrefabRegistry.h"
#include "Duin/Script/ScriptContext.h"
#include "Duin/Core/Maths/DuinMaths.h"

//#include "./dn_ecs.das.inc"

// ---- daScript external type factories ----
MAKE_EXTERNAL_TYPE_FACTORY(ecs_world_t, ecs_world_t)

// ---- Direct type bindings ----

DAS_TYPE_DECL(dn_Vector3, duin::Vector3);
DAS_TYPE_DECL(dn_Quaternion, duin::Quaternion);
DAS_TYPE_DECL(dn_Position3D, duin::ECSComponent::Position3D);
DAS_TYPE_DECL(dn_Rotation3D, duin::ECSComponent::Rotation3D);
DAS_TYPE_DECL(dn_Scale3D, duin::ECSComponent::Scale3D);
DAS_TYPE_DECL(dn_Transform3D, duin::ECSComponent::Transform3D::Transform3DImpl);
DAS_TYPE_DECL(dn_Velocity3D, duin::ECSComponent::Velocity3D);

DAS_TYPE_ANNOTATION(dn_Vector3)
{
    DAS_ADD_FIELD_BIND(x);
    DAS_ADD_FIELD_BIND(y);
    DAS_ADD_FIELD_BIND(z);
}

DAS_TYPE_ANNOTATION(dn_Quaternion)
{
    DAS_ADD_FIELD_BIND(x);
    DAS_ADD_FIELD_BIND(y);
    DAS_ADD_FIELD_BIND(z);
    DAS_ADD_FIELD_BIND(w);
}

DAS_TYPE_ANNOTATION(dn_Position3D)
{
    DAS_ADD_FIELD_BIND(value);
}

DAS_TYPE_ANNOTATION(dn_Rotation3D)
{
    DAS_ADD_FIELD_BIND(value);
}

DAS_TYPE_ANNOTATION(dn_Scale3D)
{
    DAS_ADD_FIELD_BIND(value);
}

DAS_TYPE_ANNOTATION(dn_Transform3D)
{
    DAS_ADD_FIELD_BIND(pos);
    DAS_ADD_FIELD_BIND(scale);
    DAS_ADD_FIELD_BIND(rot);
}

DAS_TYPE_ANNOTATION(dn_Velocity3D)
{
    DAS_ADD_FIELD_BIND(value);
}

// ---- Factory helpers ----

static duin::Vector3 dn_make_vector3(float x, float y, float z)
{
    return duin::Vector3{x, y, z};
}

static duin::Quaternion dn_make_quaternion(float x, float y, float z, float w)
{
    return duin::Quaternion{x, y, z, w};
}

static duin::ECSComponent::Position3D dn_make_position3d(float x, float y, float z)
{
    return duin::ECSComponent::Position3D{duin::Vector3{x, y, z}};
}

static duin::ECSComponent::Rotation3D dn_make_rotation3d(float x, float y, float z, float w)
{
    return duin::ECSComponent::Rotation3D{duin::Quaternion{x, y, z, w}};
}

static duin::ECSComponent::Scale3D dn_make_scale3d(float x, float y, float z)
{
    return duin::ECSComponent::Scale3D{duin::Vector3{x, y, z}};
}

static duin::ECSComponent::Transform3D::Transform3DImpl dn_make_transform3d(float px, float py, float pz, float sx,
                                                                            float sy, float sz, float rx, float ry,
                                                                            float rz, float rw)
{
    return duin::ECSComponent::Transform3D::Transform3DImpl{duin::Vector3{px, py, pz}, duin::Vector3{sx, sy, sz},
                                                            duin::Quaternion{rx, ry, rz, rw}};
}

static duin::ECSComponent::Velocity3D dn_make_velocity3d(float x, float y, float z)
{
    return duin::ECSComponent::Velocity3D{duin::Vector3{x, y, z}};
}

// ---- Component ID lookups (ecs_world_t* → ecs_entity_t) ----

static uint64_t dn_component_id_position3d(ecs_world_t *w)
{
    if (!w)
        return 0;
    flecs::world fw(w);
    return fw.component<duin::ECSComponent::Position3D>().id();
}

static uint64_t dn_component_id_rotation3d(ecs_world_t *w)
{
    if (!w)
        return 0;
    flecs::world fw(w);
    return fw.component<duin::ECSComponent::Rotation3D>().id();
}

static uint64_t dn_component_id_scale3d(ecs_world_t *w)
{
    if (!w)
        return 0;
    flecs::world fw(w);
    return fw.component<duin::ECSComponent::Scale3D>().id();
}

static uint64_t dn_component_id_transform3d(ecs_world_t *w)
{
    if (!w)
        return 0;
    flecs::world fw(w);
    return fw.component<duin::ECSComponent::Transform3D>().id();
}

static uint64_t dn_component_id_velocity3d(ecs_world_t *w)
{
    if (!w)
        return 0;
    flecs::world fw(w);
    return fw.component<duin::ECSComponent::Velocity3D>().id();
}

// ---- Tag ID lookups ----

static uint64_t dn_tag_id_local(ecs_world_t *w)
{
    if (!w)
        return 0;
    flecs::world fw(w);
    return fw.component<duin::ECSTag::Local>().id();
}

static uint64_t dn_tag_id_global(ecs_world_t *w)
{
    if (!w)
        return 0;
    flecs::world fw(w);
    return fw.component<duin::ECSTag::Global>().id();
}

static uint64_t dn_tag_id_pxkinematic(ecs_world_t *w)
{
    if (!w)
        return 0;
    flecs::world fw(w);
    return fw.component<duin::ECSTag::PxKinematic>().id();
}

static uint64_t dn_tag_id_pxdynamic(ecs_world_t *w)
{
    if (!w)
        return 0;
    flecs::world fw(w);
    return fw.component<duin::ECSTag::PxDynamic>().id();
}

static uint64_t dn_tag_id_pxstatic(ecs_world_t *w)
{
    if (!w)
        return 0;
    flecs::world fw(w);
    return fw.component<duin::ECSTag::PxStatic>().id();
}

static uint64_t dn_tag_id_nonpx(ecs_world_t *w)
{
    if (!w)
        return 0;
    flecs::world fw(w);
    return fw.component<duin::ECSTag::NonPx>().id();
}

static uint64_t dn_tag_id_set_camera_as_active(ecs_world_t *w)
{
    if (!w)
        return 0;
    flecs::world fw(w);
    return fw.component<duin::ECSTag::SetCameraAsActive>().id();
}

static uint64_t dn_tag_id_camera_is_active(ecs_world_t *w)
{
    if (!w)
        return 0;
    flecs::world fw(w);
    return fw.component<duin::ECSTag::CameraIsActive>().id();
}

static uint64_t dn_tag_id_active_camera(ecs_world_t *w)
{
    if (!w)
        return 0;
    flecs::world fw(w);
    return fw.component<duin::ECSTag::ActiveCamera>().id();
}

// ---- Component set/get (now using bound structs directly) ----

static void dn_entity_set_position3d(ecs_world_t *w, uint64_t eid, const duin::ECSComponent::Position3D &position)
{
    if (!w || !eid)
        return;
    flecs::world fw(w);
    fw.entity(eid).set<duin::ECSComponent::Position3D>(position);
}

static duin::ECSComponent::Position3D dn_entity_get_position3d(ecs_world_t *w, uint64_t eid)
{
    duin::ECSComponent::Position3D result;
    if (!w || !eid)
        return result;
    flecs::world fw(w);
    const auto *c = fw.entity(eid).try_get_mut<duin::ECSComponent::Position3D>();
    if (c)
    {
        result = *c;
    }
    return result;
}

static void dn_entity_set_rotation3d(ecs_world_t *w, uint64_t eid, const duin::ECSComponent::Rotation3D &rotation)
{
    if (!w || !eid)
        return;
    flecs::world fw(w);
    fw.entity(eid).set<duin::ECSComponent::Rotation3D>(rotation);
}

static duin::ECSComponent::Rotation3D dn_entity_get_rotation3d(ecs_world_t *w, uint64_t eid)
{
    duin::ECSComponent::Rotation3D result;
    if (!w || !eid)
        return result;
    flecs::world fw(w);
    const auto *c = fw.entity(eid).try_get_mut<duin::ECSComponent::Rotation3D>();
    if (c)
    {
        result = *c;
    }
    return result;
}

static void dn_entity_set_scale3d(ecs_world_t *w, uint64_t eid, const duin::ECSComponent::Scale3D &scale)
{
    if (!w || !eid)
        return;
    flecs::world fw(w);
    fw.entity(eid).set<duin::ECSComponent::Scale3D>(scale);
}

static duin::ECSComponent::Scale3D dn_entity_get_scale3d(ecs_world_t *w, uint64_t eid)
{
    duin::ECSComponent::Scale3D result;
    if (!w || !eid)
        return result;
    flecs::world fw(w);
    const auto *c = fw.entity(eid).try_get_mut<duin::ECSComponent::Scale3D>();
    if (c)
    {
        result = *c;
    }
    return result;
}

static void dn_entity_set_transform3d(ecs_world_t *w, uint64_t eid,
                                      const duin::ECSComponent::Transform3D::Transform3DImpl &transform)
{
    if (!w || !eid)
        return;
    flecs::world fw(w);
    fw.entity(eid).set<duin::ECSComponent::Transform3D>(duin::ECSComponent::Transform3D(transform));
}

static duin::ECSComponent::Transform3D::Transform3DImpl dn_entity_get_transform3d(ecs_world_t *w, uint64_t eid)
{
    duin::ECSComponent::Transform3D::Transform3DImpl result{duin::Vector3Zero(), duin::Vector3One(),
                                                            duin::QuaternionIdentity()};
    if (!w || !eid)
        return result;
    flecs::world fw(w);
    const auto *c = fw.entity(eid).try_get_mut<duin::ECSComponent::Transform3D>();
    if (c)
    {
        result = c->reflection();
    }
    return result;
}

static void dn_entity_set_velocity3d(ecs_world_t *w, uint64_t eid, const duin::ECSComponent::Velocity3D &velocity)
{
    if (!w || !eid)
        return;
    flecs::world fw(w);
    fw.entity(eid).set<duin::ECSComponent::Velocity3D>(velocity);
}

static duin::ECSComponent::Velocity3D dn_entity_get_velocity3d(ecs_world_t *w, uint64_t eid)
{
    duin::ECSComponent::Velocity3D result;
    if (!w || !eid)
        return result;
    flecs::world fw(w);
    const auto *c = fw.entity(eid).try_get_mut<duin::ECSComponent::Velocity3D>();
    if (c)
    {
        result = *c;
    }
    return result;
}

// ---- GameWorld externs ----

static ecs_world_t *dn_get_flecs_world_impl(das::Context *context)
{
    auto *dnCtx = static_cast<duin::ScriptContext *>(context);
    if (!dnCtx || !dnCtx->gameWorld)
        return nullptr;
    return dnCtx->gameWorld->GetFlecsWorld().c_ptr();
}

static void *dn_get_gameworld_ptr_impl(das::Context *context)
{
    auto *dnCtx = static_cast<duin::ScriptContext *>(context);
    return dnCtx ? static_cast<void *>(dnCtx->gameWorld) : nullptr;
}

static uint64_t dn_gameworld_find_prefab_impl(void *gwPtr, const char *name)
{
    if (!gwPtr || !name)
        return 0;
    duin::Entity prefab = duin::PrefabRegistry::Get().Find(name);
    return prefab.GetID();
}

static void dn_gameworld_set_global_position_impl(void *gwPtr, uint64_t entityId, float x, float y, float z)
{
    if (!gwPtr || !entityId)
        return;
    auto *gw = static_cast<duin::GameWorld *>(gwPtr);
    duin::Entity e(entityId, gw);
    gw->SetGlobalPosition(e, duin::Vector3{x, y, z});
}

static void dn_gameworld_get_global_position_impl(void *gwPtr, uint64_t entityId, float *x, float *y, float *z)
{
    if (!gwPtr || !entityId)
        return;
    auto *gw = static_cast<duin::GameWorld *>(gwPtr);
    duin::Entity e(entityId, gw);
    duin::Vector3 pos = gw->GetGlobalPosition(e);
    *x = pos.x;
    *y = pos.y;
    *z = pos.z;
}

static void dn_gameworld_set_global_rotation_impl(void *gwPtr, uint64_t entityId, float x, float y, float z, float w)
{
    if (!gwPtr || !entityId)
        return;
    auto *gw = static_cast<duin::GameWorld *>(gwPtr);
    duin::Entity e(entityId, gw);
    gw->SetGlobalRotation(e, duin::Quaternion{x, y, z, w});
}

static void dn_gameworld_get_global_rotation_impl(void *gwPtr, uint64_t entityId, float *x, float *y, float *z,
                                                  float *w)
{
    if (!gwPtr || !entityId)
        return;
    auto *gw = static_cast<duin::GameWorld *>(gwPtr);
    duin::Entity e(entityId, gw);
    duin::Quaternion rot = gw->GetGlobalRotation(e);
    *x = rot.x;
    *y = rot.y;
    *z = rot.z;
    *w = rot.w;
}

static void dn_gameworld_set_global_scale_impl(void *gwPtr, uint64_t entityId, float x, float y, float z)
{
    if (!gwPtr || !entityId)
        return;
    auto *gw = static_cast<duin::GameWorld *>(gwPtr);
    duin::Entity e(entityId, gw);
    gw->SetGlobalScale(e, duin::Vector3{x, y, z});
}

static void dn_gameworld_get_global_scale_impl(void *gwPtr, uint64_t entityId, float *x, float *y, float *z)
{
    if (!gwPtr || !entityId)
        return;
    auto *gw = static_cast<duin::GameWorld *>(gwPtr);
    duin::Entity e(entityId, gw);
    duin::Vector3 scale = gw->GetGlobalScale(e);
    *x = scale.x;
    *y = scale.y;
    *z = scale.z;
}

// ---- Module ----

class Module_DnECS : public das::Module
{
    bool initialized = false;

  public:
    Module_DnECS() : das::Module("dn_ecs_core")
    {
    }

    bool initDependencies() override
    {
        if (initialized)
            return true;
        initialized = true;

        das::ModuleLibrary lib(this);
        lib.addBuiltInModule();

        auto *logMod = das::Module::require("dn_log");
        if (!logMod)
            DN_CORE_ERROR("decs: failed to find required module 'dn_log'");
        addBuiltinDependency(lib, logMod);

        auto *flecsMod = das::Module::require("flecs_core");
        if (!flecsMod)
            DN_CORE_ERROR("decs: failed to find required module 'flecs_core'");
        addBuiltinDependency(lib, flecsMod);

        addAnnotation(new dn_Vector3Annotation(lib));
        addAnnotation(new dn_QuaternionAnnotation(lib));
        addAnnotation(new dn_Position3DAnnotation(lib));
        addAnnotation(new dn_Rotation3DAnnotation(lib));
        addAnnotation(new dn_Scale3DAnnotation(lib));
        addAnnotation(new dn_Transform3DAnnotation(lib));
        addAnnotation(new dn_Velocity3DAnnotation(lib));

        // Factory helpers
        addExtern<DAS_BIND_FUN(dn_make_vector3), das::SimNode_ExtFuncCallAndCopyOrMove>(
            *this, lib, "dn_make_vector3", das::SideEffects::none, "dn_make_vector3")
            ->args({"x", "y", "z"});
        addExtern<DAS_BIND_FUN(dn_make_quaternion), das::SimNode_ExtFuncCallAndCopyOrMove>(
            *this, lib, "dn_make_quaternion", das::SideEffects::none, "dn_make_quaternion")
            ->args({"x", "y", "z", "w"});
        addExtern<DAS_BIND_FUN(dn_make_position3d), das::SimNode_ExtFuncCallAndCopyOrMove>(
            *this, lib, "dn_make_position3d", das::SideEffects::none, "dn_make_position3d")
            ->args({"x", "y", "z"});
        addExtern<DAS_BIND_FUN(dn_make_rotation3d), das::SimNode_ExtFuncCallAndCopyOrMove>(
            *this, lib, "dn_make_rotation3d", das::SideEffects::none, "dn_make_rotation3d")
            ->args({"x", "y", "z", "w"});
        addExtern<DAS_BIND_FUN(dn_make_scale3d), das::SimNode_ExtFuncCallAndCopyOrMove>(
            *this, lib, "dn_make_scale3d", das::SideEffects::none, "dn_make_scale3d")
            ->args({"x", "y", "z"});
        addExtern<DAS_BIND_FUN(dn_make_transform3d), das::SimNode_ExtFuncCallAndCopyOrMove>(
            *this, lib, "dn_make_transform3d", das::SideEffects::none, "dn_make_transform3d")
            ->args({"px", "py", "pz", "sx", "sy", "sz", "rx", "ry", "rz", "rw"});
        addExtern<DAS_BIND_FUN(dn_make_velocity3d), das::SimNode_ExtFuncCallAndCopyOrMove>(
            *this, lib, "dn_make_velocity3d", das::SideEffects::none, "dn_make_velocity3d")
            ->args({"x", "y", "z"});

        // Component ID lookups
        addExtern<DAS_BIND_FUN(dn_component_id_position3d)>(*this, lib, "dn_component_id_position3d",
                                                            das::SideEffects::none, "dn_component_id_position3d");
        addExtern<DAS_BIND_FUN(dn_component_id_rotation3d)>(*this, lib, "dn_component_id_rotation3d",
                                                            das::SideEffects::none, "dn_component_id_rotation3d");
        addExtern<DAS_BIND_FUN(dn_component_id_scale3d)>(*this, lib, "dn_component_id_scale3d", das::SideEffects::none,
                                                         "dn_component_id_scale3d");
        addExtern<DAS_BIND_FUN(dn_component_id_transform3d)>(*this, lib, "dn_component_id_transform3d",
                                                             das::SideEffects::none, "dn_component_id_transform3d");
        addExtern<DAS_BIND_FUN(dn_component_id_velocity3d)>(*this, lib, "dn_component_id_velocity3d",
                                                            das::SideEffects::none, "dn_component_id_velocity3d");

        // Tag ID lookups
        addExtern<DAS_BIND_FUN(dn_tag_id_local)>(*this, lib, "dn_tag_id_local", das::SideEffects::none,
                                                 "dn_tag_id_local");
        addExtern<DAS_BIND_FUN(dn_tag_id_global)>(*this, lib, "dn_tag_id_global", das::SideEffects::none,
                                                  "dn_tag_id_global");
        addExtern<DAS_BIND_FUN(dn_tag_id_pxkinematic)>(*this, lib, "dn_tag_id_pxkinematic", das::SideEffects::none,
                                                       "dn_tag_id_pxkinematic");
        addExtern<DAS_BIND_FUN(dn_tag_id_pxdynamic)>(*this, lib, "dn_tag_id_pxdynamic", das::SideEffects::none,
                                                     "dn_tag_id_pxdynamic");
        addExtern<DAS_BIND_FUN(dn_tag_id_pxstatic)>(*this, lib, "dn_tag_id_pxstatic", das::SideEffects::none,
                                                    "dn_tag_id_pxstatic");
        addExtern<DAS_BIND_FUN(dn_tag_id_nonpx)>(*this, lib, "dn_tag_id_nonpx", das::SideEffects::none,
                                                 "dn_tag_id_nonpx");
        addExtern<DAS_BIND_FUN(dn_tag_id_set_camera_as_active)>(
            *this, lib, "dn_tag_id_set_camera_as_active", das::SideEffects::none, "dn_tag_id_set_camera_as_active");
        addExtern<DAS_BIND_FUN(dn_tag_id_camera_is_active)>(*this, lib, "dn_tag_id_camera_is_active",
                                                            das::SideEffects::none, "dn_tag_id_camera_is_active");
        addExtern<DAS_BIND_FUN(dn_tag_id_active_camera)>(*this, lib, "dn_tag_id_active_camera", das::SideEffects::none,
                                                         "dn_tag_id_active_camera");

        // Component set/get
        addExtern<DAS_BIND_FUN(dn_entity_set_position3d)>(*this, lib, "dn_entity_set_position3d",
                                                          das::SideEffects::modifyExternal, "dn_entity_set_position3d");
        addExtern<DAS_BIND_FUN(dn_entity_get_position3d), das::SimNode_ExtFuncCallAndCopyOrMove>(
            *this, lib, "dn_entity_get_position3d", das::SideEffects::none, "dn_entity_get_position3d");
        addExtern<DAS_BIND_FUN(dn_entity_set_rotation3d)>(*this, lib, "dn_entity_set_rotation3d",
                                                          das::SideEffects::modifyExternal, "dn_entity_set_rotation3d");
        addExtern<DAS_BIND_FUN(dn_entity_get_rotation3d), das::SimNode_ExtFuncCallAndCopyOrMove>(
            *this, lib, "dn_entity_get_rotation3d", das::SideEffects::none, "dn_entity_get_rotation3d");
        addExtern<DAS_BIND_FUN(dn_entity_set_scale3d)>(*this, lib, "dn_entity_set_scale3d",
                                                       das::SideEffects::modifyExternal, "dn_entity_set_scale3d");
        addExtern<DAS_BIND_FUN(dn_entity_get_scale3d), das::SimNode_ExtFuncCallAndCopyOrMove>(
            *this, lib, "dn_entity_get_scale3d", das::SideEffects::none, "dn_entity_get_scale3d");
        addExtern<DAS_BIND_FUN(dn_entity_set_transform3d)>(
            *this, lib, "dn_entity_set_transform3d", das::SideEffects::modifyExternal, "dn_entity_set_transform3d");
        addExtern<DAS_BIND_FUN(dn_entity_get_transform3d), das::SimNode_ExtFuncCallAndCopyOrMove>(
            *this, lib, "dn_entity_get_transform3d", das::SideEffects::none, "dn_entity_get_transform3d");
        addExtern<DAS_BIND_FUN(dn_entity_set_velocity3d)>(*this, lib, "dn_entity_set_velocity3d",
                                                          das::SideEffects::modifyExternal, "dn_entity_set_velocity3d");
        addExtern<DAS_BIND_FUN(dn_entity_get_velocity3d), das::SimNode_ExtFuncCallAndCopyOrMove>(
            *this, lib, "dn_entity_get_velocity3d", das::SideEffects::none, "dn_entity_get_velocity3d");

        //compileBuiltinModule("dn_ecs.das", dn_ecs_das, sizeof(dn_ecs_das));

        DN_CORE_INFO("Script Module [decs] initialized.");
        return true;
    }
};

REGISTER_MODULE(Module_DnECS);
REGISTER_DYN_MODULE(Module_DnECS, Module_DnECS);
