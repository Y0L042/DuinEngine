#include "dnpch.h"

#include <daScript/daScript.h>
#include <daScript/daScriptBind.h>
#include <daScript/ast/ast_typefactory_bind.h>
#include <daScript/ast/ast_interop.h>

#include "tomlc17.h"
#include "Duin/Core/Debug/DNLog.h"

// ========================================================================
// Enum binding — toml_type_t (plain C enum, no namespace)
// ========================================================================

DAS_BIND_ENUM_CAST_98_IN_NAMESPACE(toml_type_t, TomlType)
DAS_BASE_BIND_ENUM_98(
    toml_type_t, TomlType, TOML_UNKNOWN, TOML_STRING, TOML_INT64, TOML_FP64, TOML_BOOLEAN, TOML_DATE, TOML_TIME,
    TOML_DATETIME, TOML_DATETIMETZ, TOML_ARRAY, TOML_TABLE)

// ========================================================================
// toml_datum_t → TomlDatum (opaque; union accessed via free functions)
// ========================================================================

MAKE_TYPE_FACTORY(TomlDatum, toml_datum_t)

struct TomlDatumAnnotation : das::ManagedStructureAnnotation<toml_datum_t, false, false>
{
    TomlDatumAnnotation(das::ModuleLibrary &lib) : ManagedStructureAnnotation("TomlDatum", lib)
    {
        addField<DAS_BIND_MANAGED_FIELD(type)>("typev", "typev");
    }
};

// ========================================================================
// toml_result_t → TomlResult
// ========================================================================

MAKE_TYPE_FACTORY(TomlResult, toml_result_t)

struct TomlResultAnnotation : das::ManagedStructureAnnotation<toml_result_t, false, false>
{
    TomlResultAnnotation(das::ModuleLibrary &lib) : ManagedStructureAnnotation("TomlResult", lib)
    {
        addField<DAS_BIND_MANAGED_FIELD(ok)>("ok", "ok");
        addField<DAS_BIND_MANAGED_FIELD(toptab)>("toptab", "toptab");
    }
};

// ========================================================================
// Parse wrappers
// ========================================================================

static toml_result_t toml_parse_impl(const char *src)
{
    if (!src)
    {
        toml_result_t r = {};
        return r;
    }
    return toml_parse(src, (int)strlen(src));
}

static toml_result_t toml_parse_file_impl(const char *fname)
{
    return toml_parse_file_ex(fname ? fname : "");
}

// ========================================================================
// Lifecycle
// ========================================================================

static void toml_free_impl(toml_result_t &r)
{
    toml_result_t tmp = r;
    memset(&r, 0, sizeof(r));
    toml_free(tmp);
}

static const char *toml_result_errmsg_impl(const toml_result_t &r)
{
    return r.errmsg;
}

// ========================================================================
// Traversal
// ========================================================================

static toml_datum_t toml_get_impl(const toml_datum_t &table, const char *key)
{
    return toml_get(table, key ? key : "");
}

static toml_datum_t toml_seek_impl(const toml_datum_t &table, const char *key)
{
    return toml_seek(table, key ? key : "");
}

// ========================================================================
// Merge / equiv
// ========================================================================

static toml_result_t toml_merge_impl(const toml_result_t &r1, const toml_result_t &r2)
{
    return toml_merge(&r1, &r2);
}

static bool toml_equiv_impl(const toml_result_t &r1, const toml_result_t &r2)
{
    return toml_equiv(&r1, &r2);
}

// ========================================================================
// Scalar union accessors
// ========================================================================

static const char *toml_datum_str_impl(const toml_datum_t &d)
{
    return d.type == TOML_STRING ? (d.u.s ? d.u.s : "") : nullptr;
}

static int64_t toml_datum_int_impl(const toml_datum_t &d)
{
    return d.type == TOML_INT64 ? d.u.int64 : 0;
}

static double toml_datum_fp_impl(const toml_datum_t &d)
{
    return d.type == TOML_FP64 ? d.u.fp64 : 0.0;
}

static bool toml_datum_bool_impl(const toml_datum_t &d)
{
    return d.type == TOML_BOOLEAN && d.u.boolean;
}

// ========================================================================
// Array accessors
// ========================================================================

static int32_t toml_datum_arr_size_impl(const toml_datum_t &d)
{
    return d.type == TOML_ARRAY ? d.u.arr.size : 0;
}

static toml_datum_t toml_datum_arr_elem_impl(const toml_datum_t &d, int32_t i)
{
    if (d.type == TOML_ARRAY && i >= 0 && i < d.u.arr.size)
        return d.u.arr.elem[i];
    toml_datum_t r = {};
    return r;
}

// ========================================================================
// Table accessors
// ========================================================================

static int32_t toml_datum_tab_size_impl(const toml_datum_t &d)
{
    return d.type == TOML_TABLE ? d.u.tab.size : 0;
}

static const char *toml_datum_tab_key_impl(const toml_datum_t &d, int32_t i)
{
    if (d.type == TOML_TABLE && i >= 0 && i < d.u.tab.size)
        return d.u.tab.key[i];
    return nullptr;
}

static toml_datum_t toml_datum_tab_value_impl(const toml_datum_t &d, int32_t i)
{
    if (d.type == TOML_TABLE && i >= 0 && i < d.u.tab.size)
        return d.u.tab.value[i];
    toml_datum_t r = {};
    return r;
}

// ========================================================================
// Timestamp field accessors
// ========================================================================

static int16_t toml_datum_ts_year_impl(const toml_datum_t &d)
{
    return d.u.ts.year;
}
static int16_t toml_datum_ts_month_impl(const toml_datum_t &d)
{
    return d.u.ts.month;
}
static int16_t toml_datum_ts_day_impl(const toml_datum_t &d)
{
    return d.u.ts.day;
}
static int16_t toml_datum_ts_hour_impl(const toml_datum_t &d)
{
    return d.u.ts.hour;
}
static int16_t toml_datum_ts_minute_impl(const toml_datum_t &d)
{
    return d.u.ts.minute;
}
static int16_t toml_datum_ts_second_impl(const toml_datum_t &d)
{
    return d.u.ts.second;
}
static int32_t toml_datum_ts_usec_impl(const toml_datum_t &d)
{
    return d.u.ts.usec;
}
static int16_t toml_datum_ts_tz_impl(const toml_datum_t &d)
{
    return d.u.ts.tz;
}

// ========================================================================
// Module — registers as "tomlc17_core"
// ========================================================================

class Module_TOMLC17 : public das::Module
{
    bool initialized = false;

  public:
    Module_TOMLC17() : das::Module("tomlc17_core")
    {
    }

    bool initDependencies() override
    {
        if (initialized)
            return true;
        initialized = true;

        das::ModuleLibrary lib(this);
        lib.addBuiltInModule();

        addEnumeration(new EnumerationTomlType());

        addAnnotation(new TomlDatumAnnotation(lib));
        addAnnotation(new TomlResultAnnotation(lib));

        das::addExtern<DAS_BIND_FUN(toml_parse_impl), das::SimNode_ExtFuncCallAndCopyOrMove>(
            *this, lib, "toml_parse", das::SideEffects::modifyExternal, "toml_parse_impl")
            ->args({"src"});
        das::addExtern<DAS_BIND_FUN(toml_parse_file_impl), das::SimNode_ExtFuncCallAndCopyOrMove>(
            *this, lib, "toml_parse_file", das::SideEffects::modifyExternal, "toml_parse_file_impl")
            ->args({"fname"});
        das::addExtern<DAS_BIND_FUN(toml_free_impl)>(
            *this, lib, "toml_free", das::SideEffects::modifyArgument, "toml_free_impl")
            ->args({"result"});
        das::addExtern<DAS_BIND_FUN(toml_result_errmsg_impl)>(
            *this, lib, "toml_result_errmsg", das::SideEffects::none, "toml_result_errmsg_impl")
            ->args({"result"});
        das::addExtern<DAS_BIND_FUN(toml_get_impl), das::SimNode_ExtFuncCallAndCopyOrMove>(
            *this, lib, "toml_get", das::SideEffects::none, "toml_get_impl")
            ->args({"table", "key"});
        das::addExtern<DAS_BIND_FUN(toml_seek_impl), das::SimNode_ExtFuncCallAndCopyOrMove>(
            *this, lib, "toml_seek", das::SideEffects::none, "toml_seek_impl")
            ->args({"table", "key"});
        das::addExtern<DAS_BIND_FUN(toml_merge_impl), das::SimNode_ExtFuncCallAndCopyOrMove>(
            *this, lib, "toml_merge", das::SideEffects::none, "toml_merge_impl")
            ->args({"r1", "r2"});
        das::addExtern<DAS_BIND_FUN(toml_equiv_impl)>(
            *this, lib, "toml_equiv", das::SideEffects::none, "toml_equiv_impl")
            ->args({"r1", "r2"});

        das::addExtern<DAS_BIND_FUN(toml_datum_str_impl)>(
            *this, lib, "toml_datum_str", das::SideEffects::none, "toml_datum_str_impl")
            ->args({"datum"});
        das::addExtern<DAS_BIND_FUN(toml_datum_int_impl)>(
            *this, lib, "toml_datum_int", das::SideEffects::none, "toml_datum_int_impl")
            ->args({"datum"});
        das::addExtern<DAS_BIND_FUN(toml_datum_fp_impl)>(
            *this, lib, "toml_datum_fp", das::SideEffects::none, "toml_datum_fp_impl")
            ->args({"datum"});
        das::addExtern<DAS_BIND_FUN(toml_datum_bool_impl)>(
            *this, lib, "toml_datum_bool", das::SideEffects::none, "toml_datum_bool_impl")
            ->args({"datum"});
        das::addExtern<DAS_BIND_FUN(toml_datum_arr_size_impl)>(
            *this, lib, "toml_datum_arr_size", das::SideEffects::none, "toml_datum_arr_size_impl")
            ->args({"datum"});
        das::addExtern<DAS_BIND_FUN(toml_datum_arr_elem_impl), das::SimNode_ExtFuncCallAndCopyOrMove>(
            *this, lib, "toml_datum_arr_elem", das::SideEffects::none, "toml_datum_arr_elem_impl")
            ->args({"datum", "index"});
        das::addExtern<DAS_BIND_FUN(toml_datum_tab_size_impl)>(
            *this, lib, "toml_datum_tab_size", das::SideEffects::none, "toml_datum_tab_size_impl")
            ->args({"datum"});
        das::addExtern<DAS_BIND_FUN(toml_datum_tab_key_impl)>(
            *this, lib, "toml_datum_tab_key", das::SideEffects::none, "toml_datum_tab_key_impl")
            ->args({"datum", "index"});
        das::addExtern<DAS_BIND_FUN(toml_datum_tab_value_impl), das::SimNode_ExtFuncCallAndCopyOrMove>(
            *this, lib, "toml_datum_tab_value", das::SideEffects::none, "toml_datum_tab_value_impl")
            ->args({"datum", "index"});
        das::addExtern<DAS_BIND_FUN(toml_datum_ts_year_impl)>(
            *this, lib, "toml_datum_ts_year", das::SideEffects::none, "toml_datum_ts_year_impl")
            ->args({"datum"});
        das::addExtern<DAS_BIND_FUN(toml_datum_ts_month_impl)>(
            *this, lib, "toml_datum_ts_month", das::SideEffects::none, "toml_datum_ts_month_impl")
            ->args({"datum"});
        das::addExtern<DAS_BIND_FUN(toml_datum_ts_day_impl)>(
            *this, lib, "toml_datum_ts_day", das::SideEffects::none, "toml_datum_ts_day_impl")
            ->args({"datum"});
        das::addExtern<DAS_BIND_FUN(toml_datum_ts_hour_impl)>(
            *this, lib, "toml_datum_ts_hour", das::SideEffects::none, "toml_datum_ts_hour_impl")
            ->args({"datum"});
        das::addExtern<DAS_BIND_FUN(toml_datum_ts_minute_impl)>(
            *this, lib, "toml_datum_ts_minute", das::SideEffects::none, "toml_datum_ts_minute_impl")
            ->args({"datum"});
        das::addExtern<DAS_BIND_FUN(toml_datum_ts_second_impl)>(
            *this, lib, "toml_datum_ts_second", das::SideEffects::none, "toml_datum_ts_second_impl")
            ->args({"datum"});
        das::addExtern<DAS_BIND_FUN(toml_datum_ts_usec_impl)>(
            *this, lib, "toml_datum_ts_usec", das::SideEffects::none, "toml_datum_ts_usec_impl")
            ->args({"datum"});
        das::addExtern<DAS_BIND_FUN(toml_datum_ts_tz_impl)>(
            *this, lib, "toml_datum_ts_tz", das::SideEffects::none, "toml_datum_ts_tz_impl")
            ->args({"datum"});

        DN_CORE_INFO("Script Module [tomlc17_core] initialized.");
        return true;
    }
};

REGISTER_MODULE(Module_TOMLC17);
