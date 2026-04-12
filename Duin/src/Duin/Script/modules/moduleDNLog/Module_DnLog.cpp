#include "Module_DnLog.h"

#include <daScript/ast/ast_typefactory_bind.h>
#include <daScript/ast/ast_interop.h>

#include "dn_log.das.inc"

static std::string dn_log_location(const das::LineInfoArg *at, das::Context *ctx = nullptr)
{
    if (at == nullptr)
    {
        return "?";
    }
   
    if ((at->fileInfo != nullptr) && (at->fileInfo->tabSize >= 0) && (at->fileInfo->tabSize <= 1000) &&
        (at->fileInfo->name.size() > 0))
    {
        std::string desc = at->describe();
        return desc;
    }

    return std::to_string(at->line) + ":" + std::to_string(at->column);
}

static void dn_log_fatal(const char *msg, das::LineInfoArg *at)
{
    DN_FATAL("Script [{}]: \t{}", dn_log_location(at), msg ? msg : "");
}
static void dn_log_error(const char *msg, das::LineInfoArg *at)
{
    DN_ERROR("Script [{}]: \t{}", dn_log_location(at), msg ? msg : "");
}
static void dn_log_warn(const char *msg, das::LineInfoArg *at)
{
    DN_WARN("Script [{}]: \t{}", dn_log_location(at), msg ? msg : "");
}
static void dn_log_info(const char *msg, das::LineInfoArg *at)
{
    DN_INFO("Script [{}]: \t{}", dn_log_location(at), msg ? msg : "");
}
static void dn_log_trace(const char *msg, das::LineInfoArg *at)
{
    DN_TRACE("Script [{}]: \t{}", dn_log_location(at), msg ? msg : "");
}

class Module_DnLog : public das::Module
{
    bool initialized = false;

  public:
    Module_DnLog() : das::Module("dn_log")
    {
    }

    bool initDependencies() override
    {
        if (initialized)
            return true;
        initialized = true;

        das::ModuleLibrary lib(this);
        lib.addBuiltInModule();

        das::addExtern<DAS_BIND_FUN(dn_log_fatal)>(*this, lib, "dn_fatal", das::SideEffects::modifyExternal,
                                                   "dn_log_fatal");
        das::addExtern<DAS_BIND_FUN(dn_log_error)>(*this, lib, "dn_error", das::SideEffects::modifyExternal,
                                                   "dn_log_error");
        das::addExtern<DAS_BIND_FUN(dn_log_warn)>(*this, lib, "dn_warn", das::SideEffects::modifyExternal,
                                                  "dn_log_warn");
        das::addExtern<DAS_BIND_FUN(dn_log_info)>(*this, lib, "dn_info", das::SideEffects::modifyExternal,
                                                  "dn_log_info");
        das::addExtern<DAS_BIND_FUN(dn_log_trace)>(*this, lib, "dn_trace", das::SideEffects::modifyExternal,
                                                   "dn_log_trace");

        compileBuiltinModule("dn_log.das", dn_log_das, sizeof(dn_log_das));

        DN_CORE_INFO("Script Module [dn_log] initialized.");

        return true;
    }
};

REGISTER_MODULE(Module_DnLog);
