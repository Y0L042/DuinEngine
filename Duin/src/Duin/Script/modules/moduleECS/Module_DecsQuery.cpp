#include "dnpch.h"
#include "Module_DecsQuery.h"

#include <daScript/daScript.h>
#include <daScript/ast/ast_interop.h>
#include <daScript/simulate/aot.h>
#include "Duin/ECS/DECS/DECS.h"
#include "decs_query.das.inc"

// ---------------------------------------------------------------------------
// Structs mirroring dn_Term / dn_QueryDesc in daslang
// ---------------------------------------------------------------------------

//struct dn_Term
//{
//    uint64_t id = 0;
//
//    dn_Term() = default;
//    dn_Term(uint64_t id) : id(id)
//    {
//    }
//};
//
//struct dn_QueryDesc
//{
//    das::Array terms = {};
//    dn_QueryDesc() = default;
//    dn_QueryDesc(das::Array terms) : terms(terms)
//    {
//    }
//};
//
//MAKE_TYPE_FACTORY(dn_Term,      dn_Term);
//MAKE_TYPE_FACTORY(dn_QueryDesc, dn_QueryDesc);

// ---------------------------------------------------------------------------
// ManagedStructureAnnotation subclasses
// DAS_BIND_MANAGED_FIELD resolves ManagedType from the base class typedef,
// so it works inside the subclass body without any namespace wrapping.
// ---------------------------------------------------------------------------

//struct dn_TermAnnotation : das::ManagedStructureAnnotation<dn_Term, false>
//{
//    dn_TermAnnotation(das::ModuleLibrary &ml) : das::ManagedStructureAnnotation<dn_Term, false>("dn_Term", ml)
//    {
//        addField<DAS_BIND_MANAGED_FIELD(id)>("id");
//    }
//};
//
//struct dn_QueryDescAnnotation : das::ManagedStructureAnnotation<dn_QueryDesc, false>
//{
//    dn_QueryDescAnnotation(das::ModuleLibrary &ml) : das::ManagedStructureAnnotation<dn_QueryDesc, false>("dn_QueryDesc", ml)
//    {
//        addField<DAS_BIND_MANAGED_FIELD(terms)>("terms");
//    }
//};

// ---------------------------------------------------------------------------
// Query lifecycle
// ---------------------------------------------------------------------------

static void *decs_query_impl(void *worldHandle, uint64_t *termIds, int32_t count)
{
    if (!worldHandle)
        return nullptr;

    auto *world = static_cast<duin::World *>(worldHandle);
    ecs_world_t *w = world->GetFlecsWorld().c_ptr();

    ecs_query_desc_t qdesc = {};
    int termIdx = 0;

    for (int i = 0; i < count && termIdx < FLECS_TERM_COUNT_MAX; ++i, ++termIdx)
    {
        qdesc.terms[termIdx].id = static_cast<ecs_id_t>(termIds[i]);
        qdesc.terms[termIdx].oper = EcsAnd;
    }

    qdesc.cache_kind = EcsQueryCacheAuto;

    ecs_query_t *q = ecs_query_init(w, &qdesc);
    if (!q)
        DN_CORE_ERROR("decs_query_impl: ecs_query_init failed");

    return static_cast<void *>(q);
}

// Callback-style iteration: invoke blk(entity_id) for every matching entity.
static void decs_query_each_impl(void *worldHandle, void *queryHandle,
                                 const das::TBlock<void, uint64_t> &blk,
                                 das::Context *context, das::LineInfoArg *at)
{
    if (!worldHandle || !queryHandle)
        return;

    auto *world = static_cast<duin::World *>(worldHandle);
    ecs_world_t *w = world->GetFlecsWorld().c_ptr();

    ecs_iter_t it = ecs_query_iter(w, static_cast<ecs_query_t *>(queryHandle));
    while (ecs_query_next(&it))
    {
        for (int i = 0; i < it.count; ++i)
        {
            uint64_t eid = static_cast<uint64_t>(it.entities[i]);
            das::das_invoke<void>::invoke(context, at, blk, eid);
        }
    }
}

static void decs_query_fini_impl(void *queryHandle)
{
    if (!queryHandle)
        return;
    ecs_query_fini(static_cast<ecs_query_t *>(queryHandle));
}

// ---------------------------------------------------------------------------
// Iteration — pull loop
// ---------------------------------------------------------------------------

static void *decs_query_iter_begin_impl(void *worldHandle, void *queryHandle)
{
    if (!worldHandle || !queryHandle)
        return nullptr;

    auto *world = static_cast<duin::World *>(worldHandle);
    ecs_world_t *w = world->GetFlecsWorld().c_ptr();

    auto *it = new ecs_iter_t(ecs_query_iter(w, static_cast<ecs_query_t *>(queryHandle)));
    return static_cast<void *>(it);
}

static bool decs_query_iter_next_impl(void *iterHandle)
{
    if (!iterHandle)
        return false;

    auto *it = static_cast<ecs_iter_t *>(iterHandle);
    if (ecs_query_next(it))
        return true;

    delete it;
    return false;
}

static void decs_query_iter_fini_impl(void *iterHandle)
{
    if (!iterHandle)
        return;
    auto *it = static_cast<ecs_iter_t *>(iterHandle);
    ecs_iter_fini(it);
    delete it;
}

static int32_t decs_iter_count_impl(void *iterHandle)
{
    if (!iterHandle)
        return 0;
    return static_cast<ecs_iter_t *>(iterHandle)->count;
}

static uint64_t decs_iter_entity_impl(void *iterHandle, int32_t index)
{
    if (!iterHandle)
        return 0;
    auto *it = static_cast<ecs_iter_t *>(iterHandle);
    if (index < 0 || index >= it->count)
        return 0;
    return static_cast<uint64_t>(it->entities[index]);
}

// ---------------------------------------------------------------------------
// Module
// ---------------------------------------------------------------------------

class Module_DecsQuery : public das::Module
{
    bool initialized = false;

  public:
    Module_DecsQuery() : das::Module("decs_query")
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
            DN_CORE_ERROR("decs_query: failed to find required module 'decs_entity'");
        addBuiltinDependency(lib, entityMod);

        auto *worldMod = das::Module::require("decs_world");
        if (!worldMod)
            DN_CORE_ERROR("decs_query: failed to find required module 'decs_world'");
        addBuiltinDependency(lib, worldMod);

        auto *compMod = das::Module::require("decs_components");
        if (!compMod)
            DN_CORE_ERROR("decs_query: failed to find required module 'decs_components'");
        addBuiltinDependency(lib, compMod);

        addExtern<DAS_BIND_FUN(decs_query_each_impl)>(*this, lib, "decs_query_each_impl",
                                                     das::SideEffects::modifyExternal, "decs_query_each_impl");

        addExtern<DAS_BIND_FUN(decs_query_impl)>(*this, lib, "decs_query_impl", das::SideEffects::modifyExternal,
                                                 "decs_query_impl");

        addExtern<DAS_BIND_FUN(decs_query_fini_impl)>(*this, lib, "decs_query_fini_impl",
                                                      das::SideEffects::modifyExternal, "decs_query_fini_impl");

        addExtern<DAS_BIND_FUN(decs_query_iter_begin_impl)>(
            *this, lib, "decs_query_iter_begin_impl", das::SideEffects::modifyExternal, "decs_query_iter_begin_impl");

        addExtern<DAS_BIND_FUN(decs_query_iter_next_impl)>(
            *this, lib, "decs_query_iter_next_impl", das::SideEffects::modifyExternal, "decs_query_iter_next_impl");

        addExtern<DAS_BIND_FUN(decs_query_iter_fini_impl)>(
            *this, lib, "decs_query_iter_fini_impl", das::SideEffects::modifyExternal, "decs_query_iter_fini_impl");

        addExtern<DAS_BIND_FUN(decs_iter_count_impl)>(*this, lib, "decs_iter_count_impl", das::SideEffects::none,
                                                      "decs_iter_count_impl");

        addExtern<DAS_BIND_FUN(decs_iter_entity_impl)>(*this, lib, "decs_iter_entity_impl", das::SideEffects::none,
                                                       "decs_iter_entity_impl");

        compileBuiltinModule("decs_query.das", decs_query_das, sizeof(decs_query_das));

        DN_CORE_INFO("Script Module [decs_query] initialized.");

        return true;
    }
};

REGISTER_MODULE(Module_DecsQuery);
