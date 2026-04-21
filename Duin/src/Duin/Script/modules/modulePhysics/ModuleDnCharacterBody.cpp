#include "dnpch.h"
#include "ModuleDnCharacterBody.h"

#include <daScript/daScript.h>
#include <daScript/daScriptBind.h>
#include <daScript/misc/vectypes.h>

#include "Duin/Physics/jolt/CharacterBody.h"
#include "Duin/Physics/jolt/CollisionShape.h"
#include "Duin/Core/Debug/DNLog.h"
#include "Duin/Script/DaslangConversionHelpers.h"
#include "Duin/Script/ScriptContext.h"
#include "Duin/Script/ScriptMemory.h"

// --- Helpers ---

static duin::CharacterBody *get_body(void *handle)
{
    return static_cast<duin::CharacterBody *>(handle);
}

// --- Create / Destroy ---

static void *dn_character_body_capsule_create_impl(
    float mass, float maxStrength, float maxSlopeAngle, bool enableStairStepping, bool enableStickToFloor,
    bool playerCanPushOtherCharacters, bool otherCharactersCanPushPlayer, float height, float radius,
    das::Context *context)
{
    duin::CharacterBodyDesc bodyDesc;
    bodyDesc.mass = mass;
    bodyDesc.maxStrength = maxStrength;
    bodyDesc.maxSlopeAngle = maxSlopeAngle;
    bodyDesc.enableStairStepping = enableStairStepping;
    bodyDesc.enableStickToFloor = enableStickToFloor;
    bodyDesc.playerCanPushOtherCharacters = playerCanPushOtherCharacters;
    bodyDesc.otherCharactersCanPushPlayer = otherCharactersCanPushPlayer;

    auto obj = std::make_shared<duin::CharacterBody>(bodyDesc, duin::PxCapsule{height, radius}, duin::Vector3{});

    auto *dnCtx = static_cast<duin::ScriptContext *>(context);
    return static_cast<void *>(dnCtx->scriptMemory->Add(obj));
}

static void *dn_character_body_sphere_create_impl(
    float mass, float maxStrength, float maxSlopeAngle, bool enableStairStepping, bool enableStickToFloor,
    bool playerCanPushOtherCharacters, bool otherCharactersCanPushPlayer, float radius, das::Context *context)
{
    duin::CharacterBodyDesc bodyDesc;
    bodyDesc.mass = mass;
    bodyDesc.maxStrength = maxStrength;
    bodyDesc.maxSlopeAngle = maxSlopeAngle;
    bodyDesc.enableStairStepping = enableStairStepping;
    bodyDesc.enableStickToFloor = enableStickToFloor;
    bodyDesc.playerCanPushOtherCharacters = playerCanPushOtherCharacters;
    bodyDesc.otherCharactersCanPushPlayer = otherCharactersCanPushPlayer;

    auto obj = std::make_shared<duin::CharacterBody>(bodyDesc, duin::PxSphere{radius}, duin::Vector3{});

    auto *dnCtx = static_cast<duin::ScriptContext *>(context);
    return static_cast<void *>(dnCtx->scriptMemory->Add(obj));
}

static void *dn_character_body_box_create_impl(
    float mass, float maxStrength, float maxSlopeAngle, bool enableStairStepping, bool enableStickToFloor,
    bool playerCanPushOtherCharacters, bool otherCharactersCanPushPlayer, das::float3 sides, das::Context *context)
{
    duin::CharacterBodyDesc bodyDesc;
    bodyDesc.mass = mass;
    bodyDesc.maxStrength = maxStrength;
    bodyDesc.maxSlopeAngle = maxSlopeAngle;
    bodyDesc.enableStairStepping = enableStairStepping;
    bodyDesc.enableStickToFloor = enableStickToFloor;
    bodyDesc.playerCanPushOtherCharacters = playerCanPushOtherCharacters;
    bodyDesc.otherCharactersCanPushPlayer = otherCharactersCanPushPlayer;

    auto obj = std::make_shared<duin::CharacterBody>(bodyDesc, duin::PxBox{duin::from_f3(sides)}, duin::Vector3{});

    auto *dnCtx = static_cast<duin::ScriptContext *>(context);
    return static_cast<void *>(dnCtx->scriptMemory->Add(obj));
}

static void dn_character_body_destroy_impl(void *handle, das::Context *context)
{
    if (!handle)
        return;
    auto *dnCtx = static_cast<duin::ScriptContext *>(context);
    dnCtx->scriptMemory->Remove(handle);
}

// --- Lifecycle ---

static void dn_character_body_initialize_impl(void *handle, das::float3 position)
{
    if (!handle)
        return;
    get_body(handle)->Initialize(duin::from_f3(position));
}

// --- Position ---

static void dn_character_body_set_position_impl(void *handle, das::float3 position)
{
    if (!handle)
        return;
    get_body(handle)->SetPosition(duin::from_f3(position));
}

static das::float3 dn_character_body_get_position_impl(void *handle)
{
    if (!handle)
        return {};
    return duin::to_f3(get_body(handle)->GetPosition());
}

static das::float3 dn_character_body_get_center_of_mass_impl(void *handle)
{
    if (!handle)
        return {};
    return duin::to_f3(get_body(handle)->GetCenterOfMassPosition());
}

static void dn_character_body_set_foot_position_impl(void *handle, das::float3 position)
{
    if (!handle)
        return;
    get_body(handle)->SetFootPosition(duin::from_f3(position));
}

static das::float3 dn_character_body_get_foot_position_impl(void *handle)
{
    if (!handle)
        return {};
    return duin::to_f3(get_body(handle)->GetFootPosition());
}

// --- Velocity / Floor ---

static das::float3 dn_character_body_get_velocity_impl(void *handle)
{
    if (!handle)
        return {};
    return duin::to_f3(get_body(handle)->GetCurrentVelocity());
}

static int dn_character_body_is_on_floor_impl(void *handle)
{
    if (!handle)
        return 0;
    return get_body(handle)->IsOnFloor();
}

static int dn_character_body_is_on_floor_only_impl(void *handle)
{
    if (!handle)
        return 0;
    return get_body(handle)->IsOnFloorOnly();
}

// --- Move ---

static void dn_character_body_move_impl(void *handle, das::float3 displacement, double delta)
{
    if (!handle)
        return;
    get_body(handle)->Move(duin::from_f3(displacement), delta);
}

// ---- Module: dn_characterbody_core ----

class Module_DnCharacterBody : public das::Module
{
    bool initialized = false;

  public:
    Module_DnCharacterBody() : das::Module("dn_characterbody_core")
    {
    }

    bool initDependencies() override
    {
        if (initialized)
            return true;

        auto *logMod = das::Module::require("dn_log");
        if (!logMod)
        {
            DN_CORE_ERROR("Module_DnCharacterBody: required module 'dn_log' not found");
            return false;
        }
        if (!logMod->initDependencies())
        {
            DN_CORE_ERROR("Module_DnCharacterBody: failed to initialize dependencies of 'dn_log'");
            return false;
        }

        initialized = true;

        das::ModuleLibrary lib(this);
        lib.addBuiltInModule();
        addBuiltinDependency(lib, logMod);

        addExtern<DAS_BIND_FUN(dn_character_body_capsule_create_impl)>(
            *this,
            lib,
            "dn_character_body_capsule_create_impl",
            das::SideEffects::modifyExternal,
            "dn_character_body_capsule_create_impl")
            ->args(
                {"mass",
                 "max_strength",
                 "max_slope_angle",
                 "enable_stair_stepping",
                 "enable_stick_to_floor",
                 "player_can_push_others",
                 "others_can_push_player",
                 "height",
                 "radius",
                 "context"});

        addExtern<DAS_BIND_FUN(dn_character_body_sphere_create_impl)>(
            *this,
            lib,
            "dn_character_body_sphere_create_impl",
            das::SideEffects::modifyExternal,
            "dn_character_body_sphere_create_impl")
            ->args(
                {"mass",
                 "max_strength",
                 "max_slope_angle",
                 "enable_stair_stepping",
                 "enable_stick_to_floor",
                 "player_can_push_others",
                 "others_can_push_player",
                 "radius",
                 "context"});

        addExtern<DAS_BIND_FUN(dn_character_body_box_create_impl)>(
            *this,
            lib,
            "dn_character_body_box_create_impl",
            das::SideEffects::modifyExternal,
            "dn_character_body_box_create_impl")
            ->args(
                {"mass",
                 "max_strength",
                 "max_slope_angle",
                 "enable_stair_stepping",
                 "enable_stick_to_floor",
                 "player_can_push_others",
                 "others_can_push_player",
                 "sides",
                 "context"});

        addExtern<DAS_BIND_FUN(dn_character_body_destroy_impl)>(
            *this,
            lib,
            "dn_character_body_destroy_impl",
            das::SideEffects::modifyExternal,
            "dn_character_body_destroy_impl")
            ->args({"handle", "context"});

        addExtern<DAS_BIND_FUN(dn_character_body_initialize_impl)>(
            *this,
            lib,
            "dn_character_body_initialize_impl",
            das::SideEffects::modifyExternal,
            "dn_character_body_initialize_impl")
            ->args({"handle", "position"});

        addExtern<DAS_BIND_FUN(dn_character_body_set_position_impl)>(
            *this,
            lib,
            "dn_character_body_set_position_impl",
            das::SideEffects::modifyExternal,
            "dn_character_body_set_position_impl")
            ->args({"handle", "position"});

        addExtern<DAS_BIND_FUN(dn_character_body_get_position_impl)>(
            *this,
            lib,
            "dn_character_body_get_position_impl",
            das::SideEffects::none,
            "dn_character_body_get_position_impl")
            ->args({"handle"});

        addExtern<DAS_BIND_FUN(dn_character_body_get_center_of_mass_impl)>(
            *this,
            lib,
            "dn_character_body_get_center_of_mass_impl",
            das::SideEffects::none,
            "dn_character_body_get_center_of_mass_impl")
            ->args({"handle"});

        addExtern<DAS_BIND_FUN(dn_character_body_set_foot_position_impl)>(
            *this,
            lib,
            "dn_character_body_set_foot_position_impl",
            das::SideEffects::modifyExternal,
            "dn_character_body_set_foot_position_impl")
            ->args({"handle", "position"});

        addExtern<DAS_BIND_FUN(dn_character_body_get_foot_position_impl)>(
            *this,
            lib,
            "dn_character_body_get_foot_position_impl",
            das::SideEffects::none,
            "dn_character_body_get_foot_position_impl")
            ->args({"handle"});

        addExtern<DAS_BIND_FUN(dn_character_body_get_velocity_impl)>(
            *this,
            lib,
            "dn_character_body_get_velocity_impl",
            das::SideEffects::none,
            "dn_character_body_get_velocity_impl")
            ->args({"handle"});

        addExtern<DAS_BIND_FUN(dn_character_body_is_on_floor_impl)>(
            *this,
            lib,
            "dn_character_body_is_on_floor_impl",
            das::SideEffects::none,
            "dn_character_body_is_on_floor_impl")
            ->args({"handle"});

        addExtern<DAS_BIND_FUN(dn_character_body_is_on_floor_only_impl)>(
            *this,
            lib,
            "dn_character_body_is_on_floor_only_impl",
            das::SideEffects::none,
            "dn_character_body_is_on_floor_only_impl")
            ->args({"handle"});

        addExtern<DAS_BIND_FUN(dn_character_body_move_impl)>(
            *this, lib, "dn_character_body_move_impl", das::SideEffects::modifyExternal, "dn_character_body_move_impl")
            ->args({"handle", "displacement", "delta"});

        DN_CORE_INFO("Script Module [dn_characterbody_core] initialized.");
        return true;
    }
};

REGISTER_MODULE(Module_DnCharacterBody);
