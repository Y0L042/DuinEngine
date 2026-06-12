#pragma once

#include <Duin/Core/Application.h>
#include <Duin/Core/Events/EventsModule.h>
#include <Duin/IO/IOModule.h>
#include <Duin/Objects/GameObject.h>
#include <Duin/Script/GameScript.h>

#include <string>

class DuinRT : public duin::Application
{
  public:
    static DuinRT &Get();

    DuinRT(int argc, char **argv);
    ~DuinRT()
    {
    }

    static void TerminationCallback(int sig);

    void SetScriptPath(const std::string& path);
    void SetDasRoot(const std::string& path);
    void SetProjectFile(const std::string& path);

    void Initialize() override;
    void Ready() override;
    void OnEvent(duin::Event event) override;
    void Update(double rDelta) override;
    void PhysicsUpdate(double pDelta) override;
    void Draw() override;
    void DrawUI() override;
    void Debug() override;

  private:
    static DuinRT *instance;
    std::shared_ptr<duin::GameScript> script;
    size_t argsCount;
    std::vector<std::string_view> args;
    std::string scriptPath;
    std::string dasRoot = "C:\\Projects\\CPP_Projects\\Duin\\Duin\\vendor\\daslang";
    std::string projectFile = "bin://.das_project";
    bool headlessMode = false;
    duin::Script::JitMode jitMode = duin::Script::JitMode::NONE;
    bool jitNoCache = false;

    void ParseArgs(const std::vector<std::string_view>& args);
};
