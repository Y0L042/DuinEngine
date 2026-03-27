#include "Module_DecsWorld.h"

#include <daScript/daScript.h>
#include "Duin/ECS/DECS/DECS.h"
#include "decs_world.das.inc"

static void *decs_create_world_impl(void *classPtr, const das::StructInfo *info, das::Context *context)
{
    auto obj = std::make_shared<duin::World>();
    return obj.get();
}

class Module_DecsWorld : public das::Module
{
    bool initialized = false;
  public:
    Module_DecsWorld() : das::Module("decs_world") {}

    bool initDependencies() override
    {
        if (initialized) return true;
        auto mod = das::Module::require("dn_log");
        if (!mod) return false;
        if (!mod->initDependencies()) return false;
        initialized = true;

        das::ModuleLibrary lib(this);
        lib.addBuiltInModule();
        addBuiltinDependency(lib, mod);

        //das::addExtern<DAS_BIND_FUN(decs_create_world)>(*this, lib, "decs_create_world", das::SideEffects::modifyExternal,
        //                                           "decs_create_world");

        compileBuiltinModule("decs_world.das", decs_world_das, sizeof(decs_world_das));
        return true;
    }
};

REGISTER_MODULE(Module_DecsWorld);
