#include "Module_DecsComponents.h"

#include <daScript/daScript.h>
#include "Duin/ECS/DECS/DECS.h"
#include "Duin/ECS/ECSComponents.h"
#include "Duin/ECS/ECSTag.h"
#include "Duin/Core/Maths/DuinMaths.h"
#include "decs_components.das.inc"

// ---- Component ID lookups ----

static uint64_t decs_component_id_position3d_impl(void *worldHandle)
{
    if (!worldHandle)
        return 0;
    auto *world = static_cast<duin::World *>(worldHandle);
    return world->GetFlecsWorld().component<duin::ECSComponent::Position3D>().id();
}

static uint64_t decs_component_id_rotation3d_impl(void *worldHandle)
{
    if (!worldHandle)
        return 0;
    auto *world = static_cast<duin::World *>(worldHandle);
    return world->GetFlecsWorld().component<duin::ECSComponent::Rotation3D>().id();
}

static uint64_t decs_component_id_scale3d_impl(void *worldHandle)
{
    if (!worldHandle)
        return 0;
    auto *world = static_cast<duin::World *>(worldHandle);
    return world->GetFlecsWorld().component<duin::ECSComponent::Scale3D>().id();
}

static uint64_t decs_component_id_transform3d_impl(void *worldHandle)
{
    if (!worldHandle)
        return 0;
    auto *world = static_cast<duin::World *>(worldHandle);
    return world->GetFlecsWorld().component<duin::ECSComponent::Transform3D>().id();
}

static uint64_t decs_component_id_velocity3d_impl(void *worldHandle)
{
    if (!worldHandle)
        return 0;
    auto *world = static_cast<duin::World *>(worldHandle);
    return world->GetFlecsWorld().component<duin::ECSComponent::Velocity3D>().id();
}

// ---- Tag ID lookups ----

static uint64_t decs_tag_id_local_impl(void *worldHandle)
{
    if (!worldHandle)
        return 0;
    auto *world = static_cast<duin::World *>(worldHandle);
    return world->GetFlecsWorld().component<duin::ECSTag::Local>().id();
}

static uint64_t decs_tag_id_global_impl(void *worldHandle)
{
    if (!worldHandle)
        return 0;
    auto *world = static_cast<duin::World *>(worldHandle);
    return world->GetFlecsWorld().component<duin::ECSTag::Global>().id();
}

static uint64_t decs_tag_id_pxkinematic_impl(void *worldHandle)
{
    if (!worldHandle)
        return 0;
    auto *world = static_cast<duin::World *>(worldHandle);
    return world->GetFlecsWorld().component<duin::ECSTag::PxKinematic>().id();
}

static uint64_t decs_tag_id_pxdynamic_impl(void *worldHandle)
{
    if (!worldHandle)
        return 0;
    auto *world = static_cast<duin::World *>(worldHandle);
    return world->GetFlecsWorld().component<duin::ECSTag::PxDynamic>().id();
}

static uint64_t decs_tag_id_pxstatic_impl(void *worldHandle)
{
    if (!worldHandle)
        return 0;
    auto *world = static_cast<duin::World *>(worldHandle);
    return world->GetFlecsWorld().component<duin::ECSTag::PxStatic>().id();
}

static uint64_t decs_tag_id_nonpx_impl(void *worldHandle)
{
    if (!worldHandle)
        return 0;
    auto *world = static_cast<duin::World *>(worldHandle);
    return world->GetFlecsWorld().component<duin::ECSTag::NonPx>().id();
}

static uint64_t decs_tag_id_set_camera_as_active_impl(void *worldHandle)
{
    if (!worldHandle)
        return 0;
    auto *world = static_cast<duin::World *>(worldHandle);
    return world->GetFlecsWorld().component<duin::ECSTag::SetCameraAsActive>().id();
}

static uint64_t decs_tag_id_camera_is_active_impl(void *worldHandle)
{
    if (!worldHandle)
        return 0;
    auto *world = static_cast<duin::World *>(worldHandle);
    return world->GetFlecsWorld().component<duin::ECSTag::CameraIsActive>().id();
}

static uint64_t decs_tag_id_active_camera_impl(void *worldHandle)
{
    if (!worldHandle)
        return 0;
    auto *world = static_cast<duin::World *>(worldHandle);
    return world->GetFlecsWorld().component<duin::ECSTag::ActiveCamera>().id();
}

// ---- Position3D set/get ----

static void decs_entity_set_position3d_impl(void *worldHandle, uint64_t entityId, float x, float y, float z)
{
    if (!worldHandle || !entityId)
        return;
    auto *world = static_cast<duin::World *>(worldHandle);
    duin::Entity e(entityId, world);
    e.Set<duin::ECSComponent::Position3D>(duin::ECSComponent::Position3D(x, y, z));
}

static void decs_entity_get_position3d_impl(void *worldHandle, uint64_t entityId, float *x, float *y, float *z)
{
    if (!worldHandle || !entityId)
        return;
    auto *world = static_cast<duin::World *>(worldHandle);
    duin::Entity e(entityId, world);
    const auto *comp = e.TryGet<duin::ECSComponent::Position3D>();
    if (comp)
    {
        *x = comp->value.x;
        *y = comp->value.y;
        *z = comp->value.z;
    }
}

// ---- Rotation3D set/get ----

static void decs_entity_set_rotation3d_impl(void *worldHandle, uint64_t entityId, float x, float y, float z, float w)
{
    if (!worldHandle || !entityId)
        return;
    auto *world = static_cast<duin::World *>(worldHandle);
    duin::Entity e(entityId, world);
    e.Set<duin::ECSComponent::Rotation3D>(duin::ECSComponent::Rotation3D(x, y, z, w));
}

static void decs_entity_get_rotation3d_impl(void *worldHandle, uint64_t entityId, float *x, float *y, float *z,
                                            float *w)
{
    if (!worldHandle || !entityId)
        return;
    auto *world = static_cast<duin::World *>(worldHandle);
    duin::Entity e(entityId, world);
    const auto *comp = e.TryGet<duin::ECSComponent::Rotation3D>();
    if (comp)
    {
        *x = comp->value.x;
        *y = comp->value.y;
        *z = comp->value.z;
        *w = comp->value.w;
    }
}

// ---- Scale3D set/get ----

static void decs_entity_set_scale3d_impl(void *worldHandle, uint64_t entityId, float x, float y, float z)
{
    if (!worldHandle || !entityId)
        return;
    auto *world = static_cast<duin::World *>(worldHandle);
    duin::Entity e(entityId, world);
    e.Set<duin::ECSComponent::Scale3D>(duin::ECSComponent::Scale3D(x, y, z));
}

static void decs_entity_get_scale3d_impl(void *worldHandle, uint64_t entityId, float *x, float *y, float *z)
{
    if (!worldHandle || !entityId)
        return;
    auto *world = static_cast<duin::World *>(worldHandle);
    duin::Entity e(entityId, world);
    const auto *comp = e.TryGet<duin::ECSComponent::Scale3D>();
    if (comp)
    {
        *x = comp->value.x;
        *y = comp->value.y;
        *z = comp->value.z;
    }
}

// ---- Transform3D set/get ----

static void decs_entity_set_transform3d_impl(void *worldHandle, uint64_t entityId, float px, float py, float pz,
                                             float sx, float sy, float sz, float rx, float ry, float rz, float rw)
{
    if (!worldHandle || !entityId)
        return;
    auto *world = static_cast<duin::World *>(worldHandle);
    duin::Entity e(entityId, world);
    duin::ECSComponent::Transform3D tx(duin::Vector3{px, py, pz}, duin::Vector3{sx, sy, sz},
                                       duin::Quaternion{rx, ry, rz, rw});
    e.Set<duin::ECSComponent::Transform3D>(tx);
}

static void decs_entity_get_transform3d_impl(void *worldHandle, uint64_t entityId, float *px, float *py, float *pz,
                                             float *sx, float *sy, float *sz, float *rx, float *ry, float *rz,
                                             float *rw)
{
    if (!worldHandle || !entityId)
        return;
    auto *world = static_cast<duin::World *>(worldHandle);
    duin::Entity e(entityId, world);
    const auto *comp = e.TryGet<duin::ECSComponent::Transform3D>();
    if (comp)
    {
        *px = comp->GetPosition().x;
        *py = comp->GetPosition().y;
        *pz = comp->GetPosition().z;
        *sx = comp->GetScale().x;
        *sy = comp->GetScale().y;
        *sz = comp->GetScale().z;
        *rx = comp->GetRotation().x;
        *ry = comp->GetRotation().y;
        *rz = comp->GetRotation().z;
        *rw = comp->GetRotation().w;
    }
}

// ---- Velocity3D set/get ----

static void decs_entity_set_velocity3d_impl(void *worldHandle, uint64_t entityId, float x, float y, float z)
{
    if (!worldHandle || !entityId)
        return;
    auto *world = static_cast<duin::World *>(worldHandle);
    duin::Entity e(entityId, world);
    e.Set<duin::ECSComponent::Velocity3D>(duin::ECSComponent::Velocity3D(x, y, z));
}

static void decs_entity_get_velocity3d_impl(void *worldHandle, uint64_t entityId, float *x, float *y, float *z)
{
    if (!worldHandle || !entityId)
        return;
    auto *world = static_cast<duin::World *>(worldHandle);
    duin::Entity e(entityId, world);
    const auto *comp = e.TryGet<duin::ECSComponent::Velocity3D>();
    if (comp)
    {
        *x = comp->value.x;
        *y = comp->value.y;
        *z = comp->value.z;
    }
}

class Module_DecsComponents : public das::Module
{
    bool initialized = false;

  public:
    Module_DecsComponents() : das::Module("decs_components")
    {
    }

    bool initDependencies() override
    {
        if (initialized)
            return true;

        initialized = true;

        das::ModuleLibrary lib(this);
        lib.addBuiltInModule();
        auto *entityMod = das::Module::require("decs_entity");
        if (!entityMod)
            DN_CORE_ERROR("decs_components: failed to find required module 'decs_entity'");
        addBuiltinDependency(lib, entityMod);

        // Component ID lookups
        addExtern<DAS_BIND_FUN(decs_component_id_position3d_impl)>(*this, lib, "decs_component_id_position3d_impl",
                                                                   das::SideEffects::none,
                                                                   "decs_component_id_position3d_impl");
        addExtern<DAS_BIND_FUN(decs_component_id_rotation3d_impl)>(*this, lib, "decs_component_id_rotation3d_impl",
                                                                   das::SideEffects::none,
                                                                   "decs_component_id_rotation3d_impl");
        addExtern<DAS_BIND_FUN(decs_component_id_scale3d_impl)>(
            *this, lib, "decs_component_id_scale3d_impl", das::SideEffects::none, "decs_component_id_scale3d_impl");
        addExtern<DAS_BIND_FUN(decs_component_id_transform3d_impl)>(*this, lib, "decs_component_id_transform3d_impl",
                                                                    das::SideEffects::none,
                                                                    "decs_component_id_transform3d_impl");
        addExtern<DAS_BIND_FUN(decs_component_id_velocity3d_impl)>(*this, lib, "decs_component_id_velocity3d_impl",
                                                                   das::SideEffects::none,
                                                                   "decs_component_id_velocity3d_impl");

        // Tag ID lookups
        addExtern<DAS_BIND_FUN(decs_tag_id_local_impl)>(*this, lib, "decs_tag_id_local_impl", das::SideEffects::none,
                                                        "decs_tag_id_local_impl");
        addExtern<DAS_BIND_FUN(decs_tag_id_global_impl)>(*this, lib, "decs_tag_id_global_impl", das::SideEffects::none,
                                                         "decs_tag_id_global_impl");
        addExtern<DAS_BIND_FUN(decs_tag_id_pxkinematic_impl)>(*this, lib, "decs_tag_id_pxkinematic_impl",
                                                              das::SideEffects::none, "decs_tag_id_pxkinematic_impl");
        addExtern<DAS_BIND_FUN(decs_tag_id_pxdynamic_impl)>(*this, lib, "decs_tag_id_pxdynamic_impl",
                                                            das::SideEffects::none, "decs_tag_id_pxdynamic_impl");
        addExtern<DAS_BIND_FUN(decs_tag_id_pxstatic_impl)>(*this, lib, "decs_tag_id_pxstatic_impl",
                                                           das::SideEffects::none, "decs_tag_id_pxstatic_impl");
        addExtern<DAS_BIND_FUN(decs_tag_id_nonpx_impl)>(*this, lib, "decs_tag_id_nonpx_impl", das::SideEffects::none,
                                                        "decs_tag_id_nonpx_impl");
        addExtern<DAS_BIND_FUN(decs_tag_id_set_camera_as_active_impl)>(
            *this, lib, "decs_tag_id_set_camera_as_active_impl", das::SideEffects::none,
            "decs_tag_id_set_camera_as_active_impl");
        addExtern<DAS_BIND_FUN(decs_tag_id_camera_is_active_impl)>(*this, lib, "decs_tag_id_camera_is_active_impl",
                                                                   das::SideEffects::none,
                                                                   "decs_tag_id_camera_is_active_impl");
        addExtern<DAS_BIND_FUN(decs_tag_id_active_camera_impl)>(
            *this, lib, "decs_tag_id_active_camera_impl", das::SideEffects::none, "decs_tag_id_active_camera_impl");

        // Entity component set/get
        addExtern<DAS_BIND_FUN(decs_entity_set_position3d_impl)>(*this, lib, "decs_entity_set_position3d_impl",
                                                                 das::SideEffects::modifyExternal,
                                                                 "decs_entity_set_position3d_impl");
        addExtern<DAS_BIND_FUN(decs_entity_get_position3d_impl)>(*this, lib, "decs_entity_get_position3d_impl",
                                                                 das::SideEffects::modifyExternal,
                                                                 "decs_entity_get_position3d_impl");
        addExtern<DAS_BIND_FUN(decs_entity_set_rotation3d_impl)>(*this, lib, "decs_entity_set_rotation3d_impl",
                                                                 das::SideEffects::modifyExternal,
                                                                 "decs_entity_set_rotation3d_impl");
        addExtern<DAS_BIND_FUN(decs_entity_get_rotation3d_impl)>(*this, lib, "decs_entity_get_rotation3d_impl",
                                                                 das::SideEffects::modifyExternal,
                                                                 "decs_entity_get_rotation3d_impl");
        addExtern<DAS_BIND_FUN(decs_entity_set_scale3d_impl)>(*this, lib, "decs_entity_set_scale3d_impl",
                                                              das::SideEffects::modifyExternal,
                                                              "decs_entity_set_scale3d_impl");
        addExtern<DAS_BIND_FUN(decs_entity_get_scale3d_impl)>(*this, lib, "decs_entity_get_scale3d_impl",
                                                              das::SideEffects::modifyExternal,
                                                              "decs_entity_get_scale3d_impl");
        addExtern<DAS_BIND_FUN(decs_entity_set_transform3d_impl)>(*this, lib, "decs_entity_set_transform3d_impl",
                                                                  das::SideEffects::modifyExternal,
                                                                  "decs_entity_set_transform3d_impl");
        addExtern<DAS_BIND_FUN(decs_entity_get_transform3d_impl)>(*this, lib, "decs_entity_get_transform3d_impl",
                                                                  das::SideEffects::modifyExternal,
                                                                  "decs_entity_get_transform3d_impl");
        addExtern<DAS_BIND_FUN(decs_entity_set_velocity3d_impl)>(*this, lib, "decs_entity_set_velocity3d_impl",
                                                                 das::SideEffects::modifyExternal,
                                                                 "decs_entity_set_velocity3d_impl");
        addExtern<DAS_BIND_FUN(decs_entity_get_velocity3d_impl)>(*this, lib, "decs_entity_get_velocity3d_impl",
                                                                 das::SideEffects::modifyExternal,
                                                                 "decs_entity_get_velocity3d_impl");

        compileBuiltinModule("decs_components.das", decs_components_das, sizeof(decs_components_das));

                DN_CORE_INFO("Script Module [decs_components] initialized.");

        return true;
    }
};

REGISTER_MODULE(Module_DecsComponents);
