#pragma once

#include <Duin/Objects/GameStateMachine.h>

class State_SelectProject : public duin::GameState
{
  public:
    State_SelectProject(duin::GameStateMachine &sm) : duin::GameState(sm) {};
    ~State_SelectProject() = default;

    void Enter() override;
    void OnEvent(duin::Event e) override;
    void Update(double delta) override;
    void PhysicsUpdate(double delta) override;
    void Draw() override;
    void DrawUI() override;
    void Exit() override;

  private:
    const std::string EDITOR_CFG_PATH = "data/editor_cfg.toml";
    const std::string PROJECT_FILE_NAME = "duin.project";
    int selectedProjectIndex = -1;
    std::vector<std::string> recentProjectDirsVec;

    void InitProjectList();
    void LoadEditorCFG();
    void ImportProject();
    void LoadSelectedProject();
    void DrawGUI();
};
