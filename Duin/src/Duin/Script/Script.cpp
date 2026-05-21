#include "dnpch.h"
#include "Script.h"
#include <Duin/Core/Debug/DNLog.h>
#include <chrono>
#include <memory>
#include <string>
#include <sstream>
#include <daScript/ast/ast.h>
#include <daScript/simulate/aot.h>
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
    DN_CORE_WARN("Compiling script {}...", scriptPath);
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
    auto compileStart = std::chrono::steady_clock::now();
    das::ProgramPtr newProgram = das::compileDaScript(scriptPath, fAccess, tout, libGroup, policies);
    auto compileMs =
        std::chrono::duration_cast<std::chrono::milliseconds>(std::chrono::steady_clock::now() - compileStart).count();
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

        DN_CORE_ERROR("Compilation failed in {}ms.", compileMs);
        res = false;
    }
    else
    {
        DN_CORE_INFO("Compiled {} in {}ms.", scriptPath, compileMs);
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

bool duin::Script::CallScript(das::Func fn, vec4f *args, void *res)
{
    if (!VerifyFunction(fn))
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
    context->evalWithCatch(fn.PTR, args, res);
    if (auto ex = context->getException())
    {
        DN_CORE_FATAL("Script Exception: \n{}\n", ex);
        context->clearException();
        return false;
    }
    return true;
}

void duin::Script::ResetToBaseModules()
{
    // Order matters here:
    // 1. libGroup.reset() first: skips promoted modules (builtIn=true, no delete) but clears
    //    the local vector — so their raw pointers are gone from libGroup before step 2.
    // 2. Module::Reset(false) then: walks the global daScriptEnvironment::modules linked list
    //    and deletes only entries where promoted==true (i.e. daslib/*.das files promoted to
    //    builtins during a previous compileDaScript). This forces re-reading them from disk so
    //    hot-reload sees any changes. Duin's own C++ modules (builtIn=true, promoted=false)
    //    are never touched by Module::Reset and survive across recompiles unchanged.
    //    Doing this after libGroup.reset() avoids a double-free: if they are deleted first,
    //    libGroup.reset() would still dereference their (now-dangling) pointers.
    libGroup.reset();
    das::Module::Reset(false);
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

bool duin::Script::InvokeVoid(das::Func fn)
{
    if (!VerifyFunction(fn))
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

    bool ok = context->runWithCatch([&]() { das::das_invoke_function<void>::invoke(context.get(), nullptr, fn); });

    if (!ok)
    {
        if (auto ex = context->getException())
        {
            DN_CORE_FATAL("Script Exception: \n{}\n", ex);
        }
        context->clearException();
        return false;
    }
    return true;
}

bool duin::Script::InvokeWithDelta(das::Func fn, double delta)
{
    if (!VerifyFunction(fn))
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

    bool ok =
        context->runWithCatch([&]() { das::das_invoke_function<void>::invoke(context.get(), nullptr, fn, delta); });
    if (!ok)
    {
        if (auto ex = context->getException())
        {
            DN_CORE_FATAL("Script Exception: \n{}\n", ex);
        }
        context->clearException();
        return false;
    }
    return true;
}

das::Func duin::Script::FindFunction(const std::string &func)
{
    if (context)
    {
        return das::Func(context->findFunction(func.c_str()));
    }
    return das::Func();
}

bool duin::Script::VerifyFunction(das::Func fn)
{
    return duin::VerifyFunction(context.get(), fn);
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

bool duin::VerifyFunction(das::Context *ctx, das::Func fn)
{
    if (!fn)
    {
        DN_CORE_FATAL("VerifyFunction: null function handle.");
    }
    if (!ctx)
    {
        DN_CORE_FATAL("VerifyFunction: null context.");
    }
    if (!ctx || !fn)
    {
        return false;
    }

    // TODO This is a temporary workaround, as it is specific to MSVC++ debug heap fill patterns.
    // Reject Windows debug-heap fill pattern (0xDDDDDDDDDDDDDDDD) so we never
    // dereference a pointer into freed memory.
    constexpr uintptr_t kFreedFill = 0xDDDDDDDDDDDDDDDDull;
    if (reinterpret_cast<uintptr_t>(fn.PTR) == kFreedFill)
    {
        DN_CORE_FATAL(
            "VerifyFunction: function pointer contains freed-memory fill (0xDDDDDDDD...). The owning context was "
            "likely destroyed.");
        return false;
    }
    // Pointer-range check: valid handles live inside the current context's
    // contiguous function table.
    const das::SimFunction *begin = ctx->getFunction(0);
    const das::SimFunction *end = begin + ctx->getTotalFunctions();
    if (fn.PTR < begin || fn.PTR >= end)
    {
        DN_CORE_FATAL(
            "VerifyFunction: function pointer {:p} is outside the context's function table [{:p}, {:p}).",
            static_cast<const void *>(fn.PTR),
            static_cast<const void *>(begin),
            static_cast<const void *>(end));
        return false;
    }
    return true;
}
