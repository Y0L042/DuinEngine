#pragma once

#include "daScript/daScript.h"
#include "Duin/ECS/GameWorld.h"
#include "../../DnGameWorldAdapter_gen.inc"
#include "Duin/Core/Debug/DNLog.h"

// =========================================================================
// Bridge: dual-inherits duin::GameWorld + generated daslang adapter.
// Created by dn_create_gameworld_impl when dn_gameworld_init is called.
// =========================================================================

class ScriptGameWorld : public duin::GameWorld, public DnGameWorldAdapterBase
{
  public:
    ScriptGameWorld(char *pClass, const das::StructInfo *info, das::Context *ctx)
        : DnGameWorldAdapterBase(info), classPtr(pClass), context(ctx)
    {
        Initialize(/*connectSignals=*/true);
    }

    void PostUpdateQueryExecution(double delta) override
    {
        duin::GameWorld::PostUpdateQueryExecution(delta);
        if (auto fn = get__post_update(classPtr))
            invoke__post_update(context, fn, classPtr, delta);
    }

    void PostPhysicsUpdateQueryExecution(double delta) override
    {
        duin::GameWorld::PostPhysicsUpdateQueryExecution(delta);
        if (auto fn = get__post_physics_update(classPtr))
            invoke__post_physics_update(context, fn, classPtr, delta);
    }

    void PostDrawQueryExecution() override
    {
        duin::GameWorld::PostDrawQueryExecution();
        if (auto fn = get__post_draw(classPtr))
            invoke__post_draw(context, fn, classPtr);
    }

    void PostDrawUIQueryExecution() override
    {
        duin::GameWorld::PostDrawUIQueryExecution();
        if (auto fn = get__post_draw_ui(classPtr))
            invoke__post_draw_ui(context, fn, classPtr);
    }

    void *classPtr;
    das::Context *context;
};
