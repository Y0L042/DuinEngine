#pragma once

#include <daScript/daScript.h>
#include <daScript/simulate/aot.h>
#include <string>
#include <functional>
#include <vector>
#include "ScriptContext.h"

namespace duin
{

bool VerifyFunction(das::Context *ctx, das::Func fn);

class Script
{
  public:
    Script();
    Script(const std::string &relScriptPath);
    ~Script();

    void SetScriptPath(const std::string &path);
    std::string GetScriptPath();
    void SetDasRoot(const std::string &path);
    void SetProjectFile(const std::string &path);
    virtual void InitModules(std::function<void(void)> initModules = [](void) {});

    bool Compile();
    bool SimulateContext();
    bool CallScript(das::Func fn, vec4f *args = (vec4f *)nullptr, void *res = (void *)nullptr);
    bool InvokeVoid(das::Func fn);
    bool InvokeWithDelta(das::Func fn, double delta);
    virtual void ResetScript();

    das::Func FindFunction(const std::string &func);
    bool VerifyFunction(das::Func fn);

  protected:
    bool scriptReady = false;
    bool modulesAreInit = false;
    std::string scriptPath;
    std::string lastCompileError;
    std::string projectFile;
    das::ProgramPtr program;
    das::ModuleGroup libGroup;
    das::TextPrinter tout;
    das::FileAccessPtr fileAccess;
    std::shared_ptr<ScriptContext> context;
    std::vector<das::Module *> baseModules;

    void ResetToBaseModules();
    void ResetContext();
    std::string SafeErrorReport(const das::Error &err);
};

} // namespace duin