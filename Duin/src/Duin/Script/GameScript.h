#pragma once

#include "Duin/Objects/GameObject.h"
#include "Script.h"
#include <string>
#include <memory>
#include "external/FileWatch.h"

namespace duin
{
class GameWorld;

class GameScript : public Script, public GameObject
{
  public:
    GameScript(const std::string &relScriptPath);
    ~GameScript();

    void SetGameFunctions();
    void ResetGameFunctions();
    void ResetMuteWarningFlags();

    void SetGameWorld(GameWorld *gw);

    void EnableHotCompile(bool enable, bool halt = false);
    bool IsHotCompileEnabled();
    bool HotCompileAndSimulate();
    bool SetContextRootObject();

    bool CompileAndSimulate(bool skipReady = false);

    void ResetScript() override;

    void Init() override;
    void Ready() override;
    void OnEvent(Event e) override;
    void Update(double delta) override;
    void PhysicsUpdate(double delta) override;
    void Draw() override;
    void DrawUI() override;

  private:
    GameWorld *gameWorld_ = nullptr;
    bool hotCompileEnabled = false;
    bool haltOnCompilationFail = false;
    bool queueHotCompileFlag = false;
    std::unique_ptr<filewatch::FileWatch<std::wstring>> directoryWatch;
    int64_t scriptLastModified = 0;
    bool muteReadyWarning = false;
    bool muteUpdateWarning = false;
    bool mutePhysicsUpdateWarning = false;
    bool muteDrawWarning = false;
    bool muteDrawUIWarning = false;
    das::SimFunction *fnGameReady = nullptr;
    das::SimFunction *fnGameUpdate = nullptr;
    das::SimFunction *fnGamePhysicsUpdate = nullptr;
    das::SimFunction *fnGameDraw = nullptr;
    das::SimFunction *fnGameDrawUI = nullptr;

    void ClearScriptGameObjects();
    void RestartSGORecurse(std::shared_ptr<GameObject> child);
};
} // namespace duin