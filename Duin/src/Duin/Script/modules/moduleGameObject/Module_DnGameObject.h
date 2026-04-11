#pragma once

#include "daScript/daScript.h"

#include "Duin/Objects/GameObject.h"

#include "../../DnGameObjectAdapter_gen.inc"

#include "Duin/Core/Debug/DNLog.h"

// =========================================================================
// Layer 3: Dual-inheritance bridge (C++ GameObject + daslang adapter)
// =========================================================================

class ScriptGameObject : public duin::GameObject, public DnGameObjectAdapterBase
{
  public:
    ScriptGameObject(char *pClass, const das::StructInfo *info, das::Context *ctx)
        : DnGameObjectAdapterBase(info), classPtr(pClass), context(ctx)
    {
        DN_CORE_INFO("EnsureImpl()");
        EnsureImpl();
    }

    void Init() override
    {
        if (auto fn = get__init(classPtr))
        {
            invoke__init(context, fn, classPtr);
        }
    }

    void Ready() override
    {
        if (auto fn = get__ready(classPtr))
        {
            invoke__ready(context, fn, classPtr);
        }
    }

    void Update(double delta) override
    {
        if (auto fn = get__update(classPtr))
        {
            invoke__update(context, fn, classPtr, delta);
        }
    }

    void PhysicsUpdate(double delta) override
    {
        if (auto fn = get__physics_update(classPtr))
        {
            invoke__physics_update(context, fn, classPtr, delta);
        }
    }

    void Draw() override
    {
        if (auto fn = get__draw(classPtr))
        {
            invoke__draw(context, fn, classPtr);
        }
    }

    void DrawUI() override
    {
        if (auto fn = get__draw_ui(classPtr))
        {
            invoke__draw_ui(context, fn, classPtr);
        }
    }

    void Debug() override
    {
        if (auto fn = get__debug(classPtr))
        {
            invoke__debug(context, fn, classPtr);
        }
    }

    void *classPtr;
    das::Context *context;
  protected:
    friend class GameScript;
      
};
