#pragma once

#include <Duin/Objects/GameStateMachine.h>
#include <Duin/Core/Window/WindowCtx.h>
#include <memory>
#include <string>
#include <vector>

class MultiWindowSandbox : public duin::GameState
{
  public:
    MultiWindowSandbox(duin::GameStateMachine &sm);
    ~MultiWindowSandbox();

    void Enter() override;
    void OnEvent(duin::Event e) override;
    void Update(double delta) override;
    void PhysicsUpdate(double delta) override;
    void Draw() override;
    void DrawUI() override;
    void Exit() override;
    void SetPause() override;
    void SetUnpause() override;

    void Tests();

  private:
    struct SandboxWindow
    {
        std::shared_ptr<duin::WindowCtx> window;
        std::string title;
        float boxHeight = 0.5f;
    };

    std::vector<std::shared_ptr<SandboxWindow>> windows;

    void OpenWindow(const std::string &title);
    bool CanDraw(const SandboxWindow &sandboxWindow);
};
