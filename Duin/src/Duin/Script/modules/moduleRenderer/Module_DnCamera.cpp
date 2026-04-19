#include "dnpch.h"
#include "Module_DnCamera.h"

#include <daScript/daScript.h>
#include <daScript/daScriptBind.h>
#include <daScript/misc/vectypes.h>

#include "Duin/Render/Camera.h"
#include "Duin/Core/Debug/DNLog.h"
#include "Duin/Script/ScriptContext.h"
#include "Duin/Script/DaslangConversionHelpers.h"

// ---- Helpers ----

static duin::CameraImpl make_impl(const das::float3 &position, const das::float3 &target, const das::float3 &globalUp,
                                  float fovy)
{
    duin::CameraImpl impl;
    impl.position = {position.x, position.y, position.z};
    impl.target = {target.x, target.y, target.z};
    impl.globalUp = {globalUp.x, globalUp.y, globalUp.z};
    impl.fovy = fovy;
    return impl;
}

// Camera class

static void *dn_camera_init_impl(das::Context *context)
{
    auto obj = std::make_shared<duin::Camera>();
    duin::ScriptContext *dnCtx = static_cast<duin::ScriptContext *>(context);
    void *handle = static_cast<void *>(dnCtx->scriptMemory->Add(obj));

    return handle;
}

static void dn_camera_finalize_impl(void *handle, das::Context *context)
{
    duin::ScriptContext *dnCtx = static_cast<duin::ScriptContext *>(context);
    dnCtx->scriptMemory->Remove(handle);
}

static void *dn_get_active_camera_impl()
{
    return duin::GetActiveCamera();
}

static void dn_set_active_camera_impl(void *dasCam)
{
    duin::Camera *camera = nullptr;

    if (dasCam != nullptr)
    {
        camera = (duin::Camera *)dasCam;
    }
    else
    {
        DN_CORE_WARN("Active camera set to nullptr, will revert to placeholder camera.");
    }
    duin::SetActiveCamera(camera);
}

static void dn_camera_get_position_impl(void *handle, das::float3 &position)
{
    if (!handle)
    {
        DN_CORE_WARN("dn_camera_get_position_impl: null camera handle");
        return;
    }
    duin::Camera *cam = static_cast<duin::Camera *>(handle);
    position = duin::to_f3(cam->GetPosition());
}

static void dn_camera_set_position_impl(void *handle, das::float3 position)
{
    if (!handle)
    {
        DN_CORE_WARN("dn_camera_set_position_impl: null camera handle");
        return;
    }
    duin::Camera *cam = static_cast<duin::Camera *>(handle);
    cam->SetPosition(duin::from_f3(position));
}

static void dn_camera_get_target_impl(void *handle, das::float3 &target)
{
    if (!handle)
    {
        DN_CORE_WARN("dn_camera_get_target_impl: null camera handle");
        return;
    }
    duin::Camera *cam = static_cast<duin::Camera *>(handle);
    target = duin::to_f3(cam->GetTarget());
}

static void dn_camera_set_target_impl(void *handle, das::float3 target)
{
    if (!handle)
    {
        DN_CORE_WARN("dn_camera_set_target_impl: null camera handle");
        return;
    }
    duin::Camera *cam = static_cast<duin::Camera *>(handle);
    cam->SetTarget(duin::from_f3(target));
}

static void dn_camera_get_global_up_impl(void *handle, das::float3 &globalUp)
{
    if (!handle)
    {
        DN_CORE_WARN("dn_camera_get_global_up_impl: null camera handle");
        return;
    }
    duin::Camera *cam = static_cast<duin::Camera *>(handle);
    globalUp = duin::to_f3(cam->GetGlobalUp());
}

static void dn_camera_set_global_up_impl(void *handle, das::float3 globalUp)
{
    if (!handle)
    {
        DN_CORE_WARN("dn_camera_set_global_up_impl: null camera handle");
        return;
    }
    duin::Camera *cam = static_cast<duin::Camera *>(handle);
    cam->SetGlobalUp(duin::from_f3(globalUp));
}

static float dn_camera_get_fovy_impl(void *handle)
{
    if (!handle)
    {
        DN_CORE_WARN("dn_camera_get_fovy_impl: null camera handle");
        return 0.0f;
    }
    duin::Camera *cam = static_cast<duin::Camera *>(handle);
    return cam->GetFOVY();
}

static void dn_camera_set_fovy_impl(void *handle, float fovy)
{
    if (!handle)
    {
        DN_CORE_WARN("dn_camera_set_fovy_impl: null camera handle");
        return;
    }
    duin::Camera *cam = static_cast<duin::Camera *>(handle);
    cam->SetFOVY(fovy);
}

static void dn_get_camera_cameraImpl_impl(void *handle, das::float3 &position, das::float3 &target,
                                          das::float3 &globalUp, float &fovy)
{
    if (!handle)
    {
        DN_CORE_WARN("dn_get_camera_cameraImpl_impl: null camera handle");
        return;
    }
    duin::Camera *cam = static_cast<duin::Camera *>(handle);
    duin::CameraImpl &impl = cam->GetImpl();

    position = duin::to_f3(impl.position);
    target = duin::to_f3(impl.target);
    globalUp = duin::to_f3(impl.globalUp);
    fovy = impl.fovy;
}

static void dn_set_camera_cameraImpl_impl(void *handle, das::float3 position, das::float3 target, das::float3 globalUp,
                                          float fovy)
{
    if (!handle)
    {
        DN_CORE_WARN("dn_set_camera_cameraImpl_impl: null camera handle");
        return;
    }
    duin::Camera *cam = static_cast<duin::Camera *>(handle);
    duin::CameraImpl &impl = cam->GetImpl();

    impl.position = duin::from_f3(position);
    impl.target = duin::from_f3(target);
    impl.globalUp = duin::from_f3(globalUp);
    impl.fovy = fovy;
}

static void dn_camera_move_forward_impl(void *handle, float distance)
{
    if (!handle)
    {
        DN_CORE_WARN("dn_camera_move_forward_impl: null camera handle");
        return;
    }
    duin::Camera *cam = static_cast<duin::Camera *>(handle);
    cam->MoveForward(distance);
}

static void dn_camera_move_up_impl(void *handle, float distance)
{
    if (!handle)
    {
        DN_CORE_WARN("dn_camera_move_up_impl: null camera handle");
        return;
    }
    duin::Camera *cam = static_cast<duin::Camera *>(handle);
    cam->MoveUp(distance);
}

static void dn_camera_move_right_impl(void *handle, float distance)
{
    if (!handle)
    {
        DN_CORE_WARN("dn_camera_move_right_impl: null camera handle");
        return;
    }
    duin::Camera *cam = static_cast<duin::Camera *>(handle);
    cam->MoveRight(distance);
}

static void dn_camera_move_to_target_impl(void *handle, float delta)
{
    if (!handle)
    {
        DN_CORE_WARN("dn_camera_move_to_target_impl: null camera handle");
        return;
    }
    duin::Camera *cam = static_cast<duin::Camera *>(handle);
    cam->MoveToTarget(delta);
}

static void dn_camera_yaw_impl(void *handle, float angle, bool rotateAroundTarget)
{
    if (!handle)
    {
        DN_CORE_WARN("dn_camera_yaw_impl: null camera handle");
        return;
    }
    duin::Camera *cam = static_cast<duin::Camera *>(handle);
    cam->Yaw(angle, rotateAroundTarget);
}

static void dn_camera_pitch_impl(void *handle, float angle, bool lockView, bool rotateAroundTarget, bool rotateUp)
{
    if (!handle)
    {
        DN_CORE_WARN("dn_camera_pitch_impl: null camera handle");
        return;
    }
    duin::Camera *cam = static_cast<duin::Camera *>(handle);
    cam->Pitch(angle, lockView, rotateAroundTarget, rotateUp);
}

static void dn_camera_roll_impl(void *handle, float angle)
{
    if (!handle)
    {
        DN_CORE_WARN("dn_camera_roll_impl: null camera handle");
        return;
    }
    duin::Camera *cam = static_cast<duin::Camera *>(handle);
    cam->Roll(angle);
}

static das::float3 dn_camera_forward_impl(void *handle)
{
    if (!handle)
    {
        DN_CORE_WARN("dn_camera_forward_impl: null camera handle");
        return {};
    }
    duin::Camera *cam = static_cast<duin::Camera *>(handle);
    return duin::to_f3(cam->Forward());
}

static das::float3 dn_camera_right_impl(void *handle)
{
    if (!handle)
    {
        DN_CORE_WARN("dn_camera_right_impl: null camera handle");
        return {};
    }
    duin::Camera *cam = static_cast<duin::Camera *>(handle);
    return duin::to_f3(cam->Right());
}

static das::float3 dn_camera_up_impl(void *handle)
{
    if (!handle)
    {
        DN_CORE_WARN("dn_camera_up_impl: null camera handle");
        return {};
    }
    duin::Camera *cam = static_cast<duin::Camera *>(handle);
    return duin::to_f3(cam->Up());
}

static bool dn_camera_is_valid_impl(void *handle)
{
    if (!handle)
    {
        DN_CORE_WARN("dn_camera_is_valid_impl: null camera handle");
        return false;
    }
    duin::Camera *cam = static_cast<duin::Camera *>(handle);
    return cam->IsValid() != 0;
}

// CameraImpl struct

// ---- Active camera accessor ----
static bool dn_get_active_cameraImpl_impl(das::float3 &position, das::float3 &target, das::float3 &globalUp,
                                          float &fovy)
{
    auto *cam = duin::GetActiveCamera();
    if (!cam)
    {
        DN_CORE_WARN("dn_get_active_cameraImpl_impl: active camera is null");
        return false;
    }
    const auto &impl = cam->GetImpl();
    position = duin::to_f3(impl.position);
    target = duin::to_f3(impl.target);
    globalUp = duin::to_f3(impl.globalUp);
    fovy = impl.fovy;
    return true;
}

static void dn_set_active_cameraImpl_impl(das::float3 &position, das::float3 &target, das::float3 &globalUp,
                                          float &fovy)
{
    auto *cam = duin::GetActiveCamera();
    if (!cam)
    {
        DN_CORE_WARN("dn_set_active_cameraImpl_impl: active camera is null");
        return;
    }
    auto &impl = cam->GetImpl();
    impl.position = duin::from_f3(position);
    impl.target = duin::from_f3(target);
    impl.globalUp = duin::from_f3(globalUp);
    impl.fovy = fovy;
}

// ---- Direction queries ----

static das::float3 dn_cameraImpl_forward_impl(const das::float3 &position, const das::float3 &target,
                                              const das::float3 &globalUp, const float &fovy)
{
    return duin::to_f3(duin::CameraForward(make_impl(position, target, globalUp, fovy)));
}

static das::float3 dn_cameraImpl_right_impl(const das::float3 &position, const das::float3 &target,
                                            const das::float3 &globalUp, const float &fovy)
{
    return duin::to_f3(duin::CameraRight(make_impl(position, target, globalUp, fovy)));
}

static das::float3 dn_cameraImpl_up_impl(const das::float3 &position, const das::float3 &target,
                                         const das::float3 &globalUp, const float &fovy)
{
    return duin::to_f3(duin::CameraUp(make_impl(position, target, globalUp, fovy)));
}

static bool dn_cameraImpl_is_valid_impl(const das::float3 &position, const das::float3 &target,
                                        const das::float3 &globalUp, const float &fovy)
{
    return duin::IsCameraValid(make_impl(position, target, globalUp, fovy));
}

// ---- Movement ----

static void dn_cameraImpl_move_forward_impl(das::float3 &position, das::float3 &target, const das::float3 &globalUp,
                                            float fovy, float distance)
{
    auto impl = make_impl(position, target, globalUp, fovy);
    duin::CameraMoveForward(impl, distance);
    position = duin::to_f3(impl.position);
    target = duin::to_f3(impl.target);
}

static void dn_cameraImpl_move_up_impl(das::float3 &position, das::float3 &target, const das::float3 &globalUp,
                                       float fovy, float distance)
{
    auto impl = make_impl(position, target, globalUp, fovy);
    duin::CameraMoveUp(impl, distance);
    position = duin::to_f3(impl.position);
    target = duin::to_f3(impl.target);
}

static void dn_cameraImpl_move_right_impl(das::float3 &position, das::float3 &target, const das::float3 &globalUp,
                                          float fovy, float distance)
{
    auto impl = make_impl(position, target, globalUp, fovy);
    duin::CameraMoveRight(impl, distance);
    position = duin::to_f3(impl.position);
    target = duin::to_f3(impl.target);
}

static void dn_cameraImpl_move_to_target_impl(das::float3 &position, das::float3 &target, const das::float3 &globalUp,
                                              float fovy, float delta)
{
    auto impl = make_impl(position, target, globalUp, fovy);
    duin::CameraMoveToTarget(impl, delta);
    position = duin::to_f3(impl.position);
    target = duin::to_f3(impl.target);
}

// ---- Rotation ----

static void dn_cameraImpl_yaw_impl(das::float3 &position, das::float3 &target, das::float3 &globalUp, float fovy,
                                   float angle, bool rotateAroundTarget)
{
    auto impl = make_impl(position, target, globalUp, fovy);
    duin::CameraYaw(impl, angle, rotateAroundTarget);
    position = duin::to_f3(impl.position);
    target = duin::to_f3(impl.target);
    globalUp = duin::to_f3(impl.globalUp);
}

static void dn_cameraImpl_pitch_impl(das::float3 &position, das::float3 &target, das::float3 &globalUp, float fovy,
                                     float angle, bool lockView, bool rotateAroundTarget, bool rotateUp)
{
    auto impl = make_impl(position, target, globalUp, fovy);
    duin::CameraPitch(impl, angle, lockView, rotateAroundTarget, rotateUp);
    position = duin::to_f3(impl.position);
    target = duin::to_f3(impl.target);
    globalUp = duin::to_f3(impl.globalUp);
}

static void dn_cameraImpl_roll_impl(das::float3 &position, das::float3 &target, das::float3 &globalUp, float fovy,
                                    float angle)
{
    auto impl = make_impl(position, target, globalUp, fovy);
    duin::CameraRoll(impl, angle);
    position = duin::to_f3(impl.position);
    target = duin::to_f3(impl.target);
    globalUp = duin::to_f3(impl.globalUp);
}

// ---- Module: dn_camera_core ----

class Module_DnCamera : public das::Module
{
    bool initialized = false;

  public:
    Module_DnCamera() : das::Module("dn_camera_core")
    {
    }

    bool initDependencies() override
    {
        if (initialized)
            return true;

        auto *logMod = das::Module::require("dn_log");
        if (!logMod)
        {
            DN_CORE_ERROR("Module_DnCamera: required module 'dn_log' not found");
            return false;
        }
        if (!logMod->initDependencies())
        {
            DN_CORE_ERROR("Module_DnCamera: failed to initialize dependencies of 'dn_log'");
            return false;
        }

        initialized = true;

        das::ModuleLibrary lib(this);
        lib.addBuiltInModule();
        addBuiltinDependency(lib, logMod);

        // Camera class
        addExtern<DAS_BIND_FUN(dn_camera_init_impl)>(*this, lib, "dn_camera_init_impl",
                                                     das::SideEffects::modifyExternal, "dn_camera_init_impl");
        addExtern<DAS_BIND_FUN(dn_camera_finalize_impl)>(*this, lib, "dn_camera_finalize_impl",
                                                         das::SideEffects::modifyExternal, "dn_camera_finalize_impl");
        addExtern<DAS_BIND_FUN(dn_get_active_camera_impl)>(*this, lib, "dn_get_active_camera_impl",
                                                           das::SideEffects::none, "dn_get_active_camera_impl");
        addExtern<DAS_BIND_FUN(dn_set_active_camera_impl)>(
            *this, lib, "dn_set_active_camera_impl", das::SideEffects::modifyExternal, "dn_set_active_camera_impl");

        // Property getters/setters
        addExtern<DAS_BIND_FUN(dn_camera_get_position_impl)>(
            *this, lib, "dn_camera_get_position_impl", das::SideEffects::modifyArgument, "dn_camera_get_position_impl")
            ->args({"handle", "position"});
        addExtern<DAS_BIND_FUN(dn_camera_set_position_impl)>(
            *this, lib, "dn_camera_set_position_impl", das::SideEffects::modifyExternal, "dn_camera_set_position_impl")
            ->args({"handle", "position"});
        addExtern<DAS_BIND_FUN(dn_camera_get_target_impl)>(
            *this, lib, "dn_camera_get_target_impl", das::SideEffects::modifyArgument, "dn_camera_get_target_impl")
            ->args({"handle", "target"});
        addExtern<DAS_BIND_FUN(dn_camera_set_target_impl)>(
            *this, lib, "dn_camera_set_target_impl", das::SideEffects::modifyExternal, "dn_camera_set_target_impl")
            ->args({"handle", "target"});
        addExtern<DAS_BIND_FUN(dn_camera_get_global_up_impl)>(*this, lib, "dn_camera_get_global_up_impl",
                                                              das::SideEffects::modifyArgument,
                                                              "dn_camera_get_global_up_impl")
            ->args({"handle", "globalUp"});
        addExtern<DAS_BIND_FUN(dn_camera_set_global_up_impl)>(*this, lib, "dn_camera_set_global_up_impl",
                                                              das::SideEffects::modifyExternal,
                                                              "dn_camera_set_global_up_impl")
            ->args({"handle", "globalUp"});
        addExtern<DAS_BIND_FUN(dn_camera_get_fovy_impl)>(*this, lib, "dn_camera_get_fovy_impl", das::SideEffects::none,
                                                         "dn_camera_get_fovy_impl")
            ->args({"handle"});
        addExtern<DAS_BIND_FUN(dn_camera_set_fovy_impl)>(*this, lib, "dn_camera_set_fovy_impl",
                                                         das::SideEffects::modifyExternal, "dn_camera_set_fovy_impl")
            ->args({"handle", "fovy"});

        // Camera class movement
        addExtern<DAS_BIND_FUN(dn_camera_move_forward_impl)>(*this, lib, "dn_camera_move_forward_impl",
                                                             das::SideEffects::modifyExternal,
                                                             "dn_camera_move_forward_impl")
            ->args({"handle", "distance"});
        addExtern<DAS_BIND_FUN(dn_camera_move_up_impl)>(*this, lib, "dn_camera_move_up_impl",
                                                        das::SideEffects::modifyExternal, "dn_camera_move_up_impl")
            ->args({"handle", "distance"});
        addExtern<DAS_BIND_FUN(dn_camera_move_right_impl)>(*this, lib, "dn_camera_move_right_impl",
                                                           das::SideEffects::modifyExternal, "dn_camera_move_right_impl")
            ->args({"handle", "distance"});
        addExtern<DAS_BIND_FUN(dn_camera_move_to_target_impl)>(*this, lib, "dn_camera_move_to_target_impl",
                                                               das::SideEffects::modifyExternal,
                                                               "dn_camera_move_to_target_impl")
            ->args({"handle", "delta"});

        // Camera class rotation
        addExtern<DAS_BIND_FUN(dn_camera_yaw_impl)>(*this, lib, "dn_camera_yaw_impl",
                                                    das::SideEffects::modifyExternal, "dn_camera_yaw_impl")
            ->args({"handle", "angle", "rotateAroundTarget"});
        addExtern<DAS_BIND_FUN(dn_camera_pitch_impl)>(*this, lib, "dn_camera_pitch_impl",
                                                      das::SideEffects::modifyExternal, "dn_camera_pitch_impl")
            ->args({"handle", "angle", "lockView", "rotateAroundTarget", "rotateUp"});
        addExtern<DAS_BIND_FUN(dn_camera_roll_impl)>(*this, lib, "dn_camera_roll_impl",
                                                     das::SideEffects::modifyExternal, "dn_camera_roll_impl")
            ->args({"handle", "angle"});

        // Camera class direction queries
        addExtern<DAS_BIND_FUN(dn_camera_forward_impl)>(
            *this, lib, "dn_camera_forward_impl", das::SideEffects::none, "dn_camera_forward_impl")
            ->args({"handle"});
        addExtern<DAS_BIND_FUN(dn_camera_right_impl)>(
            *this, lib, "dn_camera_right_impl", das::SideEffects::none, "dn_camera_right_impl")
            ->args({"handle"});
        addExtern<DAS_BIND_FUN(dn_camera_up_impl)>(
            *this, lib, "dn_camera_up_impl", das::SideEffects::none, "dn_camera_up_impl")
            ->args({"handle"});

        // Camera class validation
        addExtern<DAS_BIND_FUN(dn_camera_is_valid_impl)>(*this, lib, "dn_camera_is_valid_impl",
                                                         das::SideEffects::none, "dn_camera_is_valid_impl")
            ->args({"handle"});

        // CameraImpl struct (get_impl / set_impl)
        addExtern<DAS_BIND_FUN(dn_get_camera_cameraImpl_impl)>(*this, lib, "dn_get_camera_cameraImpl_impl",
                                                               das::SideEffects::modifyArgument,
                                                               "dn_get_camera_cameraImpl_impl")
            ->args({"handle", "position", "target", "globalUp", "fovy"});
        addExtern<DAS_BIND_FUN(dn_set_camera_cameraImpl_impl)>(*this, lib, "dn_set_camera_cameraImpl_impl",
                                                               das::SideEffects::modifyExternal,
                                                               "dn_set_camera_cameraImpl_impl")
            ->args({"handle", "position", "target", "globalUp", "fovy"});

        // Active camera
        addExtern<DAS_BIND_FUN(dn_get_active_cameraImpl_impl)>(*this, lib, "dn_get_active_cameraImpl_impl",
                                                               das::SideEffects::modifyArgument,
                                                               "dn_get_active_cameraImpl_impl")
            ->args({"position", "target", "globalUp", "fovy"});

        addExtern<DAS_BIND_FUN(dn_set_active_cameraImpl_impl)>(*this, lib, "dn_set_active_cameraImpl_impl",
                                                               das::SideEffects::modifyExternal,
                                                               "dn_set_active_cameraImpl_impl")
            ->args({"position", "target", "globalUp", "fovy"});

        // Direction queries
        addExtern<DAS_BIND_FUN(dn_cameraImpl_forward_impl)>(
            *this, lib, "dn_cameraImpl_forward_impl", das::SideEffects::none, "dn_cameraImpl_forward_impl")
            ->args({"position", "target", "globalUp", "fovy"});
        addExtern<DAS_BIND_FUN(dn_cameraImpl_right_impl)>(
            *this, lib, "dn_cameraImpl_right_impl", das::SideEffects::none, "dn_cameraImpl_right_impl")
            ->args({"position", "target", "globalUp", "fovy"});
        addExtern<DAS_BIND_FUN(dn_cameraImpl_up_impl)>(
            *this, lib, "dn_cameraImpl_up_impl", das::SideEffects::none, "dn_cameraImpl_up_impl")
            ->args({"position", "target", "globalUp", "fovy"});

        // Validation
        addExtern<DAS_BIND_FUN(dn_cameraImpl_is_valid_impl)>(*this, lib, "dn_cameraImpl_is_valid_impl",
                                                             das::SideEffects::none, "dn_cameraImpl_is_valid_impl")
            ->args({"position", "target", "globalUp", "fovy"});

        // Movement
        addExtern<DAS_BIND_FUN(dn_cameraImpl_move_forward_impl)>(*this, lib, "dn_cameraImpl_move_forward_impl",
                                                                 das::SideEffects::modifyExternal,
                                                                 "dn_cameraImpl_move_forward_impl")
            ->args({"position", "target", "globalUp", "fovy", "distance"});
        addExtern<DAS_BIND_FUN(dn_cameraImpl_move_up_impl)>(
            *this, lib, "dn_cameraImpl_move_up_impl", das::SideEffects::modifyExternal, "dn_cameraImpl_move_up_impl")
            ->args({"position", "target", "globalUp", "fovy", "distance"});
        addExtern<DAS_BIND_FUN(dn_cameraImpl_move_right_impl)>(*this, lib, "dn_cameraImpl_move_right_impl",
                                                               das::SideEffects::modifyExternal,
                                                               "dn_cameraImpl_move_right_impl")
            ->args({"position", "target", "globalUp", "fovy", "distance"});
        addExtern<DAS_BIND_FUN(dn_cameraImpl_move_to_target_impl)>(*this, lib, "dn_cameraImpl_move_to_target_impl",
                                                                   das::SideEffects::modifyExternal,
                                                                   "dn_cameraImpl_move_to_target_impl")
            ->args({"position", "target", "globalUp", "fovy", "delta"});

        // Rotation
        addExtern<DAS_BIND_FUN(dn_cameraImpl_yaw_impl)>(*this, lib, "dn_cameraImpl_yaw_impl",
                                                        das::SideEffects::modifyExternal, "dn_cameraImpl_yaw_impl")
            ->args({"position", "target", "globalUp", "fovy", "angle", "rotateAroundTarget"});
        addExtern<DAS_BIND_FUN(dn_cameraImpl_pitch_impl)>(*this, lib, "dn_cameraImpl_pitch_impl",
                                                          das::SideEffects::modifyExternal, "dn_cameraImpl_pitch_impl")
            ->args({"position", "target", "globalUp", "fovy", "angle", "lockView", "rotateAroundTarget", "rotateUp"});
        addExtern<DAS_BIND_FUN(dn_cameraImpl_roll_impl)>(*this, lib, "dn_cameraImpl_roll_impl",
                                                         das::SideEffects::modifyExternal, "dn_cameraImpl_roll_impl")
            ->args({"position", "target", "globalUp", "fovy", "angle"});

        DN_CORE_INFO("Script Module [dn_camera_core] initialized.");
        return true;
    }
};

REGISTER_MODULE(Module_DnCamera);
