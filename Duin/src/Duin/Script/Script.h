#pragma once

#include <daScript/daScript.h>
#include <daScript/simulate/aot.h>
#include <string>
#include <functional>
#include <vector>
#include "ScriptContext.h"
#include <Duin/Core/Debug/DNAssert.h>

namespace duin
{

bool VerifyFunction(das::Context *ctx, das::Func fn);

// Controls how much of the previously-compiled module graph survives a recompile.
//   KeepCachedBindings — reuse all already-promoted .das modules (daslib + the dn_*
//     engine bindings) as-is; only the target file is re-read and recompiled. The fast
//     path for a long-lived daemon: warm-up cost is paid once, each request is cheap.
//   RefreshBindings — drop promoted modules so edited .das bindings (and daslib) are
//     re-read from disk. Needed after editing engine binding scripts; the hot-reload path.
enum class RecompileMode
{
    KeepCachedBindings,
    RefreshBindings,
};

// Structured compile diagnostic. Mirrors the fields of das::Error that tooling
// (LSP, CLI) needs: source location, severity, error code, and messages.
// Populated during Compile() alongside lastCompileError.
struct Diagnostic
{
    enum class Severity
    {
        Error,
        Warning
    };

    Severity severity = Severity::Error;
    std::string file; // empty when das::Error carried no fileInfo
    int line = 0;
    int column = 0;
    int code = 0;        // das::CompilationError (err.cerr) as int
    std::string message; // err.what
    std::string extra;   // err.extra (often empty)
    std::string fixme;   // err.fixme (suggested fix, often empty)
};

inline AssertContextScope MakeScriptContextScope(ScriptContext *ctx)
{
    return AssertContextScope([ctx]() -> std::string {
        std::string stackTop = ctx->getStackWalk(
            /*at=*/nullptr,
            /*showArguments=*/false,
            /*showLocalVariables=*/false,
            /*showOutOfScope=*/false,
            /*stackTopOnly=*/true);
        std::string fullStack = ctx->getStackWalk(
            /*at=*/nullptr,
            /*showArguments=*/true,
            /*showLocalVariables=*/true,
            /*showOutOfScope=*/false,
            /*stackTopOnly=*/false);
        return "\nFull Stack:\n" + fullStack + "\n\nStack Top:\n" + stackTop;
    });
}

// duin::Script wraps the daslang lifecycle. The public API below is ordered to follow the
// pipeline a caller drives in practice:
//   configure → InitModules → Compile → (RunLint) → SimulateContext → invoke → recompile/reset
class Script
{
  public:
    enum JitMode
    {
        NONE,
        DIRECT,
        DLL,
        EXECUTABLE
    };

    // --- Construction / destruction ---
    Script();
    Script(const std::string &relScriptPath);
    ~Script();

    // --- Configuration (call before Compile) ---
    void SetScriptPath(const std::string &path);
    std::string GetScriptPath();
    void SetDasRoot(const std::string &path);
    void SetProjectFile(const std::string &path);
    void SetProfiling(bool enable);
    void SetJitMode(JitMode mode, bool cached = true);

    // In-memory override for the script's source. When set, Compile() injects this text as
    // the source for scriptPath (via a daslang TextFileInfo) instead of reading from disk —
    // used for on-type diagnostics on UNSAVED editor buffers. Pass empty path to clear.
    void SetOverrideContent(const std::string &path, const std::string &content);
    void ClearOverrideContent();

    // --- Stage 1: Module initialization ---
    virtual void InitModules(std::function<void(void)> initModules = [](void) {});

    // --- Stage 2: Compile ---
    bool Compile();

    // Structured compile diagnostics from the most recent Compile() call.
    // Populated alongside lastCompileError; cleared at the start of Compile().
    const std::vector<Diagnostic> &GetDiagnostics() const
    {
        return diagnostics;
    }

    // --- Stage 3: Lint (optional, post-compile) ---
    // Runs daslang's C++ lint visitor on the last successfully compiled program and
    // appends any warnings it finds to `diags`. Safe to call only after Compile()
    // returns true. Parses Program::lint() text output to extract file/line/col.
    void RunLint(std::vector<Diagnostic> &diags);

    // --- Stage 4: Simulate ---
    bool SimulateContext();

    // --- Stage 5: Invoke ---
    das::Func FindFunction(const std::string &func);
    bool VerifyFunction(das::Func fn);
    bool CallScript(das::Func fn, vec4f *args = (vec4f *)nullptr, void *res = (void *)nullptr);
    bool InvokeVoid(das::Func fn);
    bool InvokeWithDelta(das::Func fn, double delta);

    // Runs all exported functions whose names begin with "test_".
    // Returns {passed, failed} counts. Each test is called via evalWithCatch;
    // a non-null exception or a panic counts as a failure.
    std::pair<int, int> RunTests();

    // --- Recompile / teardown ---
    // Prepares the same Script instance to compile a different file. Call before Compile().
    //   KeepCachedBindings (default) — reuse promoted daslib + dn_* modules; fast.
    //   RefreshBindings — re-read promoted .das modules from disk (after editing bindings).
    void PrepareForRecompile(RecompileMode mode = RecompileMode::KeepCachedBindings)
    {
        ResetToBaseModules(mode);
    }
    virtual void ResetScript();

  protected:
    // Configuration state (set via the setters above).
    bool enableProfiling = false;
    JitMode jitEnabled = JitMode::NONE;
    bool jitNoCache = false;
    std::string jitOutPath = "";
    std::string scriptPath;
    std::string projectFile;
    bool hasOverrideContent = false; // when true, compile overrideContent for overridePath
    std::string overridePath;        // native path the override applies to (usually scriptPath)
    std::string overrideContent;     // in-memory source text (unsaved buffer)

    // Runtime / pipeline state.
    bool scriptReady = false;
    bool modulesAreInit = false;
    std::string lastCompileError;
    std::vector<Diagnostic> diagnostics;
    das::ProgramPtr program;
    das::ModuleGroup libGroup;
    das::TextPrinter tout;
    das::FileAccessPtr fileAccess;
    std::shared_ptr<ScriptContext> context;
    std::vector<das::Module *> baseModules;

    // --- Compile stage helpers (used by Compile, in call order) ---
    // Rejects a missing project/source file with a 20605 diagnostic before handing paths to
    // daslang (constructing FsFileAccess on a stale URI faults inside daslang).
    bool ValidateCompileInputs();
    // Builds the FsFileAccess (project-aware when projectFile is set), mounts the "scripts"
    // root, and injects the unsaved-buffer override as a TextFileInfo when active.
    das::FileAccessPtr BuildFileAccess();
    // Assembles CodeOfPolicies: rtti/logging plus JIT mode and profiler module wiring.
    das::CodeOfPolicies BuildPolicies();
    // Shared guard for the invoke functions: verifies fn against the live context and emits
    // the null-fn / null-context warnings. Returns false if the call must not proceed.
    bool EnsureCallable(das::Func fn);

    // --- Compile error reporting helpers ---
    std::string SafeErrorReport(const das::Error &err);
    Diagnostic MakeDiagnostic(const das::Error &err);

    // --- Teardown helpers ---
    void ResetToBaseModules(RecompileMode mode = RecompileMode::KeepCachedBindings);
    void ResetContext();
};

} // namespace duin
