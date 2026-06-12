#include "dnpch.h"
#include "Script.h"
#include <Duin/Core/Debug/DNLog.h>
#include <Duin/IO/Filesystem.h>
#include <chrono>
#include <memory>
#include <algorithm>
#include <string>
#include <sstream>
#include <regex>
#include <daScript/ast/ast.h>
#include <daScript/simulate/aot.h>
#include <daScript/daScriptModule.h>
#include <daScript/misc/smart_ptr.h>
#include <daScript/misc/sysos.h>
#include <daScript/simulate/fs_file_info.h>
#include <daScript/simulate/simulate.h>
#include <daScript/ast/dyn_modules.h>
#include <vecmath/dag_vecMathDecl.h>

// =============================================================================
//  Construction / destruction
// =============================================================================

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

// =============================================================================
//  Configuration (setters) — call before Compile()
// =============================================================================

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

void duin::Script::SetProfiling(bool enable)
{
    enableProfiling = enable;
}

void duin::Script::SetJitMode(JitMode mode, bool cached)
{
    jitEnabled = mode;
    jitNoCache = !cached;
}

void duin::Script::SetOverrideContent(const std::string &path, const std::string &content)
{
    overridePath = path;
    overrideContent = content;
    hasOverrideContent = !path.empty();
}

void duin::Script::ClearOverrideContent()
{
    hasOverrideContent = false;
    overridePath.clear();
    overrideContent.clear();
}

// =============================================================================
//  Stage 1: Module initialization
// =============================================================================

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

    // Register dynamic modules (dasLLVM → the `llvm` mount that just_in_time.das requires)
    // BEFORE Module::Initialize(), like daScript/main.cpp. The mount lives in the bound
    // environment's resolve table, consulted by the file resolver at compile time.
    // Done unconditionally here because SetJitMode may run after InitModules.
    {
        das::daScriptEnvironment::ensure();
        auto bootAccess = das::make_smart<das::FsFileAccess>(); // plain access: scans dasRoot/modules literally
        std::string projectRoot;
        if (!projectFile.empty())
        {
            const std::string unix = duin::fs::EnsureUnixPath(projectFile);
            const auto slash = unix.find_last_of('/');
            if (slash != std::string::npos)
                projectRoot = unix.substr(0, slash); // dir of project file, not the file path
        }
        bool ok = das::require_dynamic_modules(bootAccess, das::getDasRoot(), projectRoot, {}, tout);
        if (!ok)
            DN_CORE_ERROR("require_dynamic_modules failed — JIT/LLVM mount may be missing.");
    }

    das::Module::Initialize();
    modulesAreInit = true;
    baseModules = libGroup.getModules();
}

// =============================================================================
//  Stage 2: Compile
//
//  Compile() drives the pipeline: validate inputs → build file access → build
//  policies → compile → handle the result. Each step is one self-describing call
//  below; the per-step detail lives in the helpers that follow.
// =============================================================================

bool duin::Script::Compile()
{
    DN_CORE_INFO("Project file set as <{}>", projectFile);
    DN_CORE_WARN("Compiling script {} ...", scriptPath);

    diagnostics.clear();

    // Step 1: reject missing project/source files before touching daslang.
    if (!ValidateCompileInputs())
        return false;

    // Step 2: build the file-access layer (project root, mounts, unsaved-buffer override).
    auto fAccess = BuildFileAccess();

    // Step 3: assemble compile policies (rtti/logging, JIT, profiler).
    das::CodeOfPolicies policies = BuildPolicies();

    // Step 4: compile. Only promote to `program` on success; time the call for logging.
    auto compileStart = std::chrono::steady_clock::now();
    das::ProgramPtr newProgram = das::compileDaScript(scriptPath, fAccess, tout, libGroup, policies);
    auto compileMs =
        std::chrono::duration_cast<std::chrono::milliseconds>(std::chrono::steady_clock::now() - compileStart).count();

    // Step 5a: compileDaScript can return a null program (e.g. the top-level file can't be
    // opened — a stale/renamed URI from an editor). Dereferencing it below would crash the
    // daemon, so synthesize a diagnostic and fail gracefully instead.
    if (!newProgram)
    {
        DN_CORE_ERROR("Compilation returned no program for {} (file missing/unopenable).", scriptPath);
        Diagnostic d;
        d.severity = Diagnostic::Severity::Error;
        d.code = 20605; // missing prerequisite / file not found
        d.message = "could not open source file: " + scriptPath;
        d.file = scriptPath;
        d.line = 1;
        d.column = 1;
        lastCompileError = d.message;
        diagnostics.push_back(d);
        return false;
    }

    // Step 5b: compilation produced a program but with errors — collect diagnostics and fail.
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
            diagnostics.push_back(MakeDiagnostic(err));
        }

        DN_CORE_ERROR("Compilation failed in {}ms.", compileMs);
        return false;
    }

    // Step 5c: success — adopt the new program.
    // Sum all "compiler took X, <file>" lines emitted by log_compile_time.
    {
        double totalDas = 0.0;
        std::istringstream ss(tout.str());
        std::string line;
        while (std::getline(ss, line))
        {
            const std::string prefix = "compiler took ";
            auto pos = line.find(prefix);
            if (pos == std::string::npos)
                continue;
            try { totalDas += std::stod(line.substr(pos + prefix.size())); } catch (...) {}
        }
        DN_CORE_INFO("Total daslang compile time: {:.6f}s", totalDas);
    }
    DN_CORE_INFO("Compiled {} in {}ms.", scriptPath, compileMs);
    program = newProgram;
    lastCompileError.clear();
    return true;
}

// Step 1 helper. Validate inputs BEFORE handing them to daslang. Constructing FsFileAccess
// on a non-existent project file faults inside daslang (it parses/runs the project's
// module_get) — an editor sending a stale/renamed URI must not crash the daemon. A missing
// script path is handled gracefully by compileDaScript, but we reject it here too for a
// cleaner diagnostic. Both checks emit a 20605 (file-not-found) diagnostic.
bool duin::Script::ValidateCompileInputs()
{
    auto fileExists = [](const std::string &p) -> bool {
        if (p.empty())
            return false;
        if (FILE *f = std::fopen(p.c_str(), "rb"))
        {
            std::fclose(f);
            return true;
        }
        return false;
    };
    auto failMissing = [&](const std::string &what, const std::string &path) -> bool {
        DN_CORE_ERROR("Cannot compile: {} not found: {}", what, path);
        Diagnostic d;
        d.severity = Diagnostic::Severity::Error;
        d.code = 20605; // missing prerequisite / file not found
        d.message = what + " not found: " + path;
        d.file = path;
        d.line = 1;
        d.column = 1;
        lastCompileError = d.message;
        diagnostics.push_back(d);
        return false;
    };

    if (!projectFile.empty() && !fileExists(projectFile))
        return failMissing("project file", projectFile);

    // When compiling in-memory override content for scriptPath, the file need not exist on
    // disk (unsaved buffer / new file) — skip the source-existence check for that path.
    const bool overrideForScript = hasOverrideContent && overridePath == scriptPath;
    if (!overrideForScript && !fileExists(scriptPath))
        return failMissing("source file", scriptPath);

    return true;
}

// Step 2 helper. Builds the FsFileAccess used for this compile and stores it in the
// `fileAccess` member (BuildPolicies() / JIT setup add extra modules to the same instance).
das::FileAccessPtr duin::Script::BuildFileAccess()
{
    // Set optional project file, script root, configure policies.
    if (!projectFile.empty())
    {
        fileAccess = das::make_smart<das::FsFileAccess>(projectFile, das::make_smart<das::FsFileAccess>());
    }
    else
    {
        fileAccess = das::make_smart<das::FsFileAccess>();
    }
    fileAccess->addFsRoot("scripts", "scripts");

    // On-type override: inject the unsaved buffer text as the source for scriptPath so daslang
    // compiles the in-memory content instead of reading the (stale or absent) file from disk.
    // TextFileInfo does NOT copy its source; with own=true its destructor frees it via
    // das_aligned_free16, so we hand it a matching das_aligned_alloc16 copy (daslang then owns
    // the buffer — no lifetime coupling to overrideContent).
    const bool overrideForScript = hasOverrideContent && overridePath == scriptPath;
    if (overrideForScript)
    {
        const uint32_t len = (uint32_t)overrideContent.size();
        char *buf = (char *)das_aligned_alloc16(len + 1);
        std::memcpy(buf, overrideContent.data(), len);
        buf[len] = '\0';
        auto info = das::make_unique<das::TextFileInfo>(buf, len, /*own=*/true);
        fileAccess->setFileInfo(scriptPath, das::move(info));
    }

    return fileAccess;
}

// Step 3 helper. Assembles the CodeOfPolicies for this compile: rtti/logging, optional JIT
// (with the matching just_in_time.das module mounted), and optional profiler.
das::CodeOfPolicies duin::Script::BuildPolicies()
{
    das::CodeOfPolicies policies;
    policies.rtti = true;
    policies.log_compile_time = true; // total time at end
    // policies.log_total_compile_time = true;  // detailed breakdown per phase
    // policies.log_module_compile_time = true; // per-module: parse / infer passes / optimize / macros

    if (jitEnabled != JitMode::NONE)
    {
        policies.jit_enabled = true;
        switch (jitEnabled)
        {
        case JitMode::EXECUTABLE:
            policies.jit_exe_mode = true;
            break;
        case JitMode::DLL:
            policies.jit_dll_mode = true;
            break;
        case JitMode::DIRECT:
            break;
        default:
            break;
        }
        if (jitNoCache)
        {
            policies.jit_dll_mode = false;
        }
        // Dynamic modules (the `llvm` mount) are registered once in InitModules().
        fileAccess->addExtraModule("just_in_time", das::getDasRoot() + "/daslib/just_in_time.das");
        policies.jit_output_path = jitOutPath;
        policies.dll_search_paths.emplace_back(das::getDasRoot() + "/lib");
        // Auto-detection uses the exe's parent folder name as the lib sub-dir, which gives
        // "DuinRT" instead of "Debug"/"Release" because our bin layout is bin/<cfg>/<proj>/.
#ifdef NDEBUG
        policies.jit_path_to_shared_lib = das::getDasRoot() + "/lib/Release/libDaScriptDyn_runtime.lib";
#else
        //policies.jit_path_to_shared_lib = das::getDasRoot() + "/lib/Debug/libDaScriptDyn_runtime.lib";
        policies.jit_path_to_shared_lib = "C:/Projects/CPP_Projects/Duin/Duin/vendor/daslang/lib/Debug/libDaScriptDyn_runtime.lib";
#endif
    }
    if (enableProfiling)
    {
        policies.profiler = true;
        fileAccess->addExtraModule("profiler", das::getDasRoot() + "/daslib/profiler.das");
    }

    return policies;
}

// --- Compile error reporting helpers ---

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

duin::Diagnostic duin::Script::MakeDiagnostic(const das::Error &err)
{
    Diagnostic d;
    d.severity = Diagnostic::Severity::Error;
    d.code = (int)err.cerr;
    d.message = err.what.c_str();
    d.extra = err.extra.c_str();
    d.fixme = err.fixme.c_str();
    d.line = (int)err.at.line;
    d.column = (int)err.at.column;

    // Guard against null fileInfo, mirroring SafeErrorReport.
    if (err.at.fileInfo != nullptr)
    {
        d.file = err.at.fileInfo->name.c_str();
    }

    return d;
}

// =============================================================================
//  Stage 3: Lint (optional, post-compile)
// =============================================================================

void duin::Script::RunLint(std::vector<Diagnostic> &diags)
{
    if (!program)
        return;
    das::TextWriter tw;
    program->lint(tw, libGroup);
    const std::string output = tw.str();
    if (output.empty())
        return;

    // Each lint line ends with " at <file>:<line>:<col>".
    // LineInfo::describe() emits  "name:line:column"  (debug_info.cpp:717).
    static const std::regex kLintLine(R"(^(.+) at (.+):(\d+):(\d+)\s*$)");
    std::istringstream ss(output);
    std::string line;
    while (std::getline(ss, line))
    {
        if (line.empty())
            continue;
        std::smatch m;
        Diagnostic d;
        d.severity = Diagnostic::Severity::Warning;
        if (std::regex_match(line, m, kLintLine))
        {
            d.message = m[1].str();
            d.file = m[2].str();
            d.line = std::stoi(m[3].str());
            d.column = std::stoi(m[4].str());
        }
        else
        {
            d.message = line;
        }
        diags.push_back(std::move(d));
    }
}

// =============================================================================
//  Stage 4: Simulate
// =============================================================================

bool duin::Script::SimulateContext()
{
    DN_CORE_INFO("Simulating context for script {}...", scriptPath);
    bool res = false;

    context = std::make_shared<ScriptContext>(
        std::max(program->getContextStackSize(), 512 * 1024)); // For daslang debugger overhead
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

// =============================================================================
//  Stage 5: Invoke
// =============================================================================

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

// Shared guard for the invoke functions below: verifies the handle against the live context
// and emits the null-fn / null-context warnings on failure. Returns false to abort the call.
bool duin::Script::EnsureCallable(das::Func fn)
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
    return true;
}

bool duin::Script::CallScript(das::Func fn, vec4f *args, void *res)
{
    if (!EnsureCallable(fn))
        return false;

    auto _dasCtx = MakeScriptContextScope(context.get());
    context->evalWithCatch(fn.PTR, args, res);

    if (auto ex = context->getException())
    {
        DN_CORE_FATAL("Script Exception: \n{}\n", ex);
        context->clearException();
        return false;
    }
    return true;
}

bool duin::Script::InvokeVoid(das::Func fn)
{
    if (!EnsureCallable(fn))
        return false;

    auto _dasCtx = MakeScriptContextScope(context.get());
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
    if (!EnsureCallable(fn))
        return false;

    auto _dasCtx = MakeScriptContextScope(context.get());
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

std::pair<int, int> duin::Script::RunTests()
{
    if (!program || !context)
        return {0, 0};

    int passed = 0, failed = 0;

    auto *mod = program->thisModule.get();
    mod->functions.foreach ([&](das::FunctionPtr &fn) {
        if (!fn || !fn->exports)
            return;
        if (fn->name.find("test_") != 0)
            return;
        // Only run 0-argument test functions.
        // [test] functions with a T? argument require a live testing::T object
        // that must be constructed daslang-side; skip them here.
        if (!fn->arguments.empty())
            return;

        const std::string name = fn->name;
        auto simFn = context->findFunction(name.c_str());
        if (!simFn)
            return;

        context->evalWithCatch(simFn, nullptr, nullptr);

        if (auto ex = context->getException())
        {
            std::cout << "  FAIL  " << name << " -- " << ex << "\n";
            context->clearException();
            ++failed;
        }
        else
        {
            std::cout << "  PASS  " << name << "\n";
            ++passed;
        }
    });

    return {passed, failed};
}

// =============================================================================
//  Recompile / teardown
// =============================================================================

void duin::Script::ResetToBaseModules(RecompileMode mode)
{
    // Step 1 (both modes): libGroup.reset() clears the group's local module vector.
    // It deletes only !builtIn modules; the promoted .das modules (builtIn==true) are
    // left alive in the global daScriptEnvironment::modules list, just dropped from the
    // group. Doing this first avoids a double-free in the RefreshBindings path below.
    libGroup.reset();

    // Step 2 (RefreshBindings only): Module::Reset(false) walks the global module list
    // and deletes every promoted==true entry — i.e. all daslib/*.das AND the dn_* engine
    // binding .das files promoted to builtins by a previous compile. This forces them to
    // be re-read from disk on the next compile, so edits to engine bindings take effect.
    // Duin's own C++ modules (builtIn==true, promoted==false) are never touched and
    // survive across recompiles either way.
    //
    // KeepCachedBindings SKIPS this: the promoted modules stay in the global list, so the
    // next compileDaScript resolves `require daslib/... | dn_...` against the already-
    // compiled modules instead of recompiling them. Only the target file is re-read. This
    // is the daemon fast path — warm graph reused, per-request cost is just the one file.
    if (mode == RecompileMode::RefreshBindings)
    {
        das::Module::Reset(false);
    }

    // Step 3 (both modes): re-add the C++ base modules (captured right after InitModules)
    // to the now-cleared group. In KeepCachedBindings the promoted .das modules remain
    // resolvable from the global list; they do not need to be in the group to be found.
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
    libGroup = das::ModuleGroup{};
    if (modulesAreInit)
    {
        das::Module::Shutdown();
        modulesAreInit = false;
    }
}

void duin::Script::ResetContext()
{
    context.reset();
    scriptReady = false;
}

// =============================================================================
//  Free functions
// =============================================================================

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
