#include "dnpch.h"
#include "Module_DnSignal.h"
#include <daScript/daScript.h>
#include <daScript/misc/handle_registry.h>
#include <daScript/ast/ast_handle.h>

#include "Duin/Core/Signals/Signal.h"

// MAKE_TYPE_FACTORY(DnScopedConnectionHandle, duin::ScopedConnection);
// static das::Handle<duin::ScopedConnection> make_scoped_connection_impl()
//{
//     auto sp = std::make_shared<duin::ScopedConnection>();
//     return das::HandleRegistry<duin::ScopedConnection>::instance().acquire(sp);
// }
//
//
//
// MAKE_TYPE_FACTORY(DnSignalHandle, duin::Signal);
// static das::Handle<duin::Signal> make_signal_impl()
//{
//     auto sp = std::make_shared<duin::Signal>();
//     return das::HandleRegistry<duin::Signal>::instance().acquire(sp);
// }
//

class Module_DnSignal : public das::Module
{
    bool initialized = false;

  public:
    Module_DnSignal() : das::Module("dn_signal_core")
    {
    }

    bool initDependencies() override
    {
        if (initialized)
        {
            return true;
        }
        initialized = true;

        das::ModuleLibrary lib(this);
        lib.addBuiltInModule();

        auto *logMod = das::Module::require("dn_log");
        if (!logMod)
            DN_CORE_ERROR("decs: failed to find required module 'dn_log'");
        addBuiltinDependency(lib, logMod);

        // addHandleAnnotation<duin::ScopedConnection>( this,
        //                             lib,
        //                             "DnScopedConnectionHandle",
        //                             "finalize", // optional — daslang destructor name
        //                             "das::Handle<duin::ScopedConnection>"); // name AOT emits into generated C++

        // addHandleAnnotation<duin::Signal>( this,
        //                             lib,
        //                             "DnSignalHandle",
        //                             "finalize", // optional — daslang destructor name
        //                             "das::Handle<duin::Signal>"); // name AOT emits into generated C++

        return true;
    }
};