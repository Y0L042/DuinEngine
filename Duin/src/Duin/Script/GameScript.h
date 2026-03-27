#pragma once

#include "Duin/Objects/GameObject.h"
#include "Script.h"
#include <string>
#include <memory>

namespace duin
{
class GameScript : public Script, public GameObject
{
  public:
    GameScript(const std::string &relScriptPath);
    ~GameScript();

    void SetGameFunctions();
    void ResetGameFunctions();
    void ResetMuteWarningFlags();

    void EnableHotCompile(bool enable, bool halt = false);
    bool IsHotCompileEnabled();
    bool HotCompileAndSimulate();

    bool CompileAndSimulate(bool skipReady = false);

    void Exit() override;

    void Init() override;
    void Ready() override;
    void Update(double delta) override;
    void PhysicsUpdate(double delta) override;
    void Draw() override;
    void DrawUI() override;

  private:
    bool hotCompileEnabled = false;
    bool haltOnCompilationFail = false;
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

    void RestartScriptGameObjects();
    void RestartSGORecurse(std::shared_ptr<GameObject> child);
};
} // namespace duin