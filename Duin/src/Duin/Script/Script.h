#pragma once

#include <daScript/daScript.h>
#include <string>
#include <functional>
#include <vector>
#include "ScriptContext.h"

namespace duin
{



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
    bool CallScript(das::SimFunction *fn, vec4f *args = (vec4f *)nullptr, void *res = (void *)nullptr);
    virtual void ResetScript();


    das::SimFunction *FindFunction(const std::string &func);

  protected:
    std::string scriptPath;

    std::shared_ptr<ScriptContext> context;

    das::ProgramPtr program;
    das::ModuleGroup libGroup;
    das::TextPrinter tout;
    das::FileAccessPtr fileAccess;
    bool scriptReady = false;
    std::string projectFile;
    bool modulesAreInit = false;
    std::vector<das::Module *> baseModules;

    void ResetToBaseModules();
    void ResetContext();
    std::string SafeErrorReport(const das::Error &err);
};

} // namespace duin