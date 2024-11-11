#pragma once

#include <Duin.h>

class StateLevelEditor : public duin::GameState
{
public:
   StateLevelEditor(duin::GameStateMachine& owner);
    ~StateLevelEditor() override;

private:
    void State_Enter();
    void State_Exit();
    void State_HandleInput();
    void State_Update(double delta);
    void State_PhysicsUpdate(double delta);
    void State_Draw();
    void State_DrawUI();
    void LoadCurrentProject();

};
