#pragma once

#include <Duin.h>

class StateStartupMenu : public duin::GameState
{
public:
    StateStartupMenu(duin::GameStateMachine& owner);
    ~StateStartupMenu() override;

private:
    void State_Enter();
    void State_Exit();
    void State_HandleInput();
    void State_Update(double delta);
    void State_PhysicsUpdate(double delta);
    void State_Draw();
    void State_DrawUI();
    void LoadCurrentProject();
    void DrawGUI();
    void LoadEditorCFG();
    void ImportProject();

};
