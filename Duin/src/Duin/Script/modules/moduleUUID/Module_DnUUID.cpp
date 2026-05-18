#include "dnpch.h"

#include <daScript/daScript.h>
#include <daScript/daScriptBind.h>
#include <daScript/ast/ast_interop.h>

#include "Duin/Core/Utils/UUID.h"
#include "Duin/Core/Debug/DNLog.h"

// ========================================================================
// Wrappers
// ========================================================================

static uint64_t dn_uuid_make_impl()
{
    return static_cast<uint64_t>(duin::UUID());
}

static uint64_t dn_uuid_invalid_impl()
{
    return static_cast<uint64_t>(duin::UUID::INVALID);
}

static bool dn_uuid_is_valid_impl(uint64_t uuid)
{
    return static_cast<bool>(duin::UUID(uuid));
}

static uint64_t dn_uuid_value_impl(uint64_t uuid)
{
    return uuid;
}

static uint64_t dn_uuid_from_value_impl(uint64_t value)
{
    return value;
}

static const char *dn_uuid_to_string_dec_impl(uint64_t uuid, das::Context *ctx)
{
    return ctx->allocateString(duin::UUID::ToStringDec(duin::UUID(uuid)), nullptr);
}

static const char *dn_uuid_to_string_hex_impl(uint64_t uuid, das::Context *ctx)
{
    return ctx->allocateString(duin::UUID::ToStringHex(duin::UUID(uuid)), nullptr);
}

static uint64_t dn_uuid_from_string_dec_impl(const char *str)
{
    if (!str || !*str)
        return static_cast<uint64_t>(duin::UUID::INVALID);
    return static_cast<uint64_t>(duin::UUID::FromStringDec(std::string(str)));
}

static uint64_t dn_uuid_from_string_hex_impl(const char *str)
{
    if (!str || !*str)
        return static_cast<uint64_t>(duin::UUID::INVALID);
    return static_cast<uint64_t>(duin::UUID::FromStringHex(std::string(str)));
}

// ========================================================================
// Module
// ========================================================================

class Module_DnUUID : public das::Module
{
    bool initialized = false;

  public:
    Module_DnUUID() : das::Module("dn_uuid_core")
    {
    }

    bool initDependencies() override
    {
        if (initialized)
            return true;
        initialized = true;

        das::ModuleLibrary lib(this);
        lib.addBuiltInModule();

        das::addExtern<DAS_BIND_FUN(dn_uuid_make_impl)>(
            *this, lib, "dn_uuid_make", das::SideEffects::modifyArgumentAndExternal, "dn_uuid_make_impl");

        das::addExtern<DAS_BIND_FUN(dn_uuid_invalid_impl)>(
            *this, lib, "dn_uuid_invalid", das::SideEffects::none, "dn_uuid_invalid_impl");

        das::addExtern<DAS_BIND_FUN(dn_uuid_is_valid_impl)>(
            *this, lib, "dn_uuid_is_valid", das::SideEffects::none, "dn_uuid_is_valid_impl")
            ->args({"uuid"});

        das::addExtern<DAS_BIND_FUN(dn_uuid_value_impl)>(
            *this, lib, "dn_uuid_value", das::SideEffects::none, "dn_uuid_value_impl")
            ->args({"uuid"});

        das::addExtern<DAS_BIND_FUN(dn_uuid_from_value_impl)>(
            *this, lib, "dn_uuid_from_value", das::SideEffects::none, "dn_uuid_from_value_impl")
            ->args({"value"});

        das::addExtern<DAS_BIND_FUN(dn_uuid_to_string_dec_impl)>(
            *this, lib, "dn_uuid_to_string_dec", das::SideEffects::none, "dn_uuid_to_string_dec_impl")
            ->args({"uuid", "context"});

        das::addExtern<DAS_BIND_FUN(dn_uuid_to_string_hex_impl)>(
            *this, lib, "dn_uuid_to_string_hex", das::SideEffects::none, "dn_uuid_to_string_hex_impl")
            ->args({"uuid", "context"});

        das::addExtern<DAS_BIND_FUN(dn_uuid_from_string_dec_impl)>(
            *this, lib, "dn_uuid_from_string_dec", das::SideEffects::none, "dn_uuid_from_string_dec_impl")
            ->args({"str"});

        das::addExtern<DAS_BIND_FUN(dn_uuid_from_string_hex_impl)>(
            *this, lib, "dn_uuid_from_string_hex", das::SideEffects::none, "dn_uuid_from_string_hex_impl")
            ->args({"str"});

        DN_CORE_INFO("Script Module [dn_uuid_core] initialized.");
        return true;
    }
};

REGISTER_MODULE(Module_DnUUID);
