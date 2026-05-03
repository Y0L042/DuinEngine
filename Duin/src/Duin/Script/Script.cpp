#include "dnpch.h"
#include "Script.h"
#include <Duin/Core/Debug/DNLog.h>
#include <memory>
#include <string>
#include <sstream>
#include <daScript/ast/ast.h>
#include <daScript/daScriptModule.h>
#include <daScript/misc/smart_ptr.h>
#include <daScript/misc/sysos.h>
#include <daScript/simulate/fs_file_info.h>
#include <daScript/simulate/runtime_string.h>
#include <daScript/simulate/simulate.h>
#include <vecmath/dag_vecMathDecl.h>

duin::Script::Script()
{
}

duin::Script::Script(const std::string &relScriptPath) : scriptPath(relScriptPath)
{
}

duin::Script::~Script()
{
    ResetScript();
}

void duin::Script::SetScriptPath(const std::string &path)
{
    scriptPath = path;
}

std::string duin::Script::GetScriptPath()
{
    return scriptPath;
}

void duin::Script::SetDasRoot(const std::string &path)
{
    das::setDasRoot(path);
}

void duin::Script::SetProjectFile(const std::string &path)
{
    projectFile = path;
}

void duin::Script::InitModules(std::function<void(void)> initModules)
{
    NEED_ALL_DEFAULT_MODULES;

    if (initModules)
    {
        initModules();
    }
    else
    {
        DN_CORE_INFO("No script modules passed.");
    }

    das::Module::Initialize();
    modulesAreInit = true;
    baseModules = libGroup.getModules();
}

bool duin::Script::Compile()
{
    DN_CORE_INFO("Compiling script {}...", scriptPath);
    bool res = false;

    // Set optional project file, script root, configure policies
    if (!projectFile.empty())
    {
        fileAccess = das::make_smart<das::FsFileAccess>(projectFile, das::make_smart<das::FsFileAccess>());
    }
    else
    {
        fileAccess = das::make_smart<das::FsFileAccess>();
    }
    auto &fAccess = fileAccess;
    fAccess->addFsRoot("scripts", "scripts");
    das::CodeOfPolicies policies;
    policies.rtti = true;

    // Attempt compilation
    // only update script program on successful compilation
    das::ProgramPtr newProgram = das::compileDaScript(scriptPath, fAccess, tout, libGroup, policies);
    if (newProgram->failed())
    {
        DN_CORE_FATAL("Compilation failed!");
        for (auto &err : newProgram->errors)
        {
            DN_CORE_FATAL("{}", SafeErrorReport(err));
        }

        lastCompileError.clear();
        for (auto &err : newProgram->errors)
        {
            lastCompileError += SafeErrorReport(err);
        }

        res = false;
    }
    else
    {
        program = newProgram;
        lastCompileError.clear();
        res = true;
    }

    return res;
}

bool duin::Script::SimulateContext()
{
    DN_CORE_INFO("Simulating context for script {}...", scriptPath);
    bool res = false;

    context = std::make_shared<ScriptContext>(program->getContextStackSize());
    context->scriptMemory = std::make_shared<ScriptMemory>();

    if (!program->simulate(*context.get(), tout))
    {
        DN_CORE_FATAL("Simulation failed!");
        for (auto &err : program->errors)
        {
            DN_CORE_FATAL("{}", SafeErrorReport(err));
        }
        ResetContext();
        res = false;
    }
    else
    {
        scriptReady = true;
        res = true;
    }

    return res;
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

void duin::Script::ResetToBaseModules()
{
    // libGroup.reset() deletes non-builtIn (script-compiled) modules and clears the list.
    // builtIn engine modules stay alive in the global linked list, so we can re-add them.
    libGroup.reset();
    for (das::Module *m : baseModules)
    {
        libGroup.addModule(m);
    }
}

void duin::Script::ResetScript()
{
    ResetContext();
    program.reset();
    fileAccess.reset();
    // baseModules.clear();
    libGroup = das::ModuleGroup{};
    if (modulesAreInit)
    {
        das::Module::Shutdown();
        modulesAreInit = false;
    }
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