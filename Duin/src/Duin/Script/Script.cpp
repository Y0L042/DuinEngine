#include "dnpch.h"
#include "Script.h"
#include <Duin/Core/Debug/DNLog.h>
#include <memory>
#include <string>
#include <sstream>
#include <daScript/ast/ast.h>
#include <daScript/daScriptModule.h>
#include <daScript/misc/smart_ptr.h>
#include <daScript/simulate/fs_file_info.h>
#include <daScript/simulate/runtime_string.h>
#include <daScript/simulate/simulate.h>
#include <vecmath/dag_vecMathDecl.h>

duin::Script::Script(const std::string &relScriptPath) : scriptPath(relScriptPath)
{
}

duin::Script::~Script()
{
    Exit();
}

void duin::Script::InitModules(std::function<void(void)> initModules)
{
    NEED_ALL_DEFAULT_MODULES;

    initModules();

    das::Module::Initialize();
    modulesAreInit = true;
}

bool duin::Script::Compile()
{
    DN_CORE_INFO("Compiling script {}...", scriptPath);
    // auto fAccess = das::make_smart<das::FsFileAccess>();
    fileAccess = das::make_smart<das::FsFileAccess>();
    auto &fAccess = fileAccess;
    fAccess->addFsRoot("scripts", "scripts");
    das::CodeOfPolicies policies;
    policies.rtti = true;
    das::ProgramPtr newProgram = das::compileDaScript(scriptPath, fAccess, tout, libGroup, policies);
    if (newProgram->failed())
    {
        // tout << "Compilation failed:\n";
        DN_CORE_FATAL("Compilation failed!");
        for (auto &err : newProgram->errors)
        {
            DN_CORE_FATAL("{}", SafeErrorReport(err));

            // tout << das::reportError(err.at, err.what, err.extra, err.fixme, err.cerr);
        }
        return false;
    }
    program = newProgram;

    return true;
}

bool duin::Script::SimulateContext()
{
    context = std::make_shared<ScriptContext>(program->getContextStackSize());
    context->scriptMemory = std::make_shared<ScriptMemory>();
    if (!program->simulate(*context.get(), tout))
    {
        // tout << "Simulation failed:\n";
        DN_CORE_FATAL("Simulation failed!");
        for (auto &err : program->errors)
        {
            DN_CORE_FATAL("{}", SafeErrorReport(err));
            // tout << das::reportError(err.at, err.what, err.extra, err.fixme, err.cerr);
            // DN_CORE_FATAL("Simulation failed: \n{}\n{}\n{}\n{}\n{}", err.at, err.what, err.extra, err.fixme,
            // err.cerr);
        }
        ResetContext();
        return false;
    }
    scriptReady = true;

    return true;
}

bool duin::Script::CallScript(das::SimFunction *fn, vec4f *args, void *res)
{
    if (!(fn && context))
    {
        if (!fn)
        {
            DN_CORE_WARN("Called script function is null!");
        }
        if (!context)
        {
            DN_CORE_WARN("Context is not set!");
        }
        return false;
    }

    context->evalWithCatch(fn, args, res);
    if (auto ex = context->getException())
    {
        DN_CORE_FATAL("Script Exception: \n{}\n", ex);
        return false;
    }
    return true;
}

void duin::Script::Exit()
{
    ResetContext();
    program.reset();
    fileAccess.reset();
    libGroup = das::ModuleGroup{};
    if (modulesAreInit)
    {
        das::Module::Shutdown();
        modulesAreInit = false;
    }
}

void duin::Script::ResetScript()
{
    Exit();
}

das::SimFunction *duin::Script::FindFunction(const std::string &func)
{
    if (context)
    {
        return context->findFunction(func.c_str());
    }
    return nullptr;
}

void duin::Script::ResetContext()
{
    context.reset();
    scriptReady = false;
}

std::string duin::Script::SafeErrorReport(const das::Error &err)
{
    std::ostringstream oss;

    oss << "\n";
    oss << "What: " << err.what << "\n";
    oss << "Extra: " << err.extra << "\n";
    oss << "Fixme: " << err.fixme << "\n";

    // Guard against null fileInfo
    if (err.at.fileInfo != nullptr)
    {
        try
        {
            oss << "At: " << err.at.describe() << "\n";
        }
        catch (...)
        {
            oss << "At: <describe() threw>\n";
        }
    }
    else
    {
        oss << "At: " << err.at.line << ":" << err.at.column << " <no file>\n";
    }

    oss << "Cerr: " << (int)err.cerr << "\n";

    return oss.str();
}