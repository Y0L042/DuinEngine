#pragma once

#include "Duin/Objects/GameObject.h"
#include "Script.h"
#include <string>
#include <memory>
#include "external/FileWatch.h"

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
    bool CompileAndSimulate();
    bool SetContextRootObject();

    void ResetScript() override;

    void Init() override;
    void Ready() override;
    void OnEvent(Event e) override;
    void Update(double delta) override;
    void PhysicsUpdate(double delta) override;
    void Draw() override;
    void DrawUI() override;

    void SetHotCompileFileChangeCooldown(float val);

  private:
    float HOT_COMPILE_FILE_CHANGE_COOLDOWN = 10.0f;

    bool hotCompileEnabled = false;
    bool haltOnCompilationFail = false;
    bool queueHotCompileFlag = false;
    float hotCompileFileChangeCooldownTimer = -999.999f;
    std::unique_ptr<filewatch::FileWatch<std::wstring>> directoryWatch;

    int64_t scriptLastModified = 0;
    float uptimeAccum = 0.0f;
    bool hasCompiledOnce = false;

    bool muteReadyWarning = false;
    bool muteUpdateWarning = false;
    bool mutePhysicsUpdateWarning = false;
    bool muteDrawWarning = false;
    bool muteDrawUIWarning = false;

    das::Func fnGameReady;
    das::Func fnGameUpdate;
    das::Func fnGamePhysicsUpdate;
    das::Func fnGameDraw;
    das::Func fnGameDrawUI;

    void ClearScriptGameObjects();
    void RestartSGORecurse(std::shared_ptr<GameObject> child);
};
} // namespace duin