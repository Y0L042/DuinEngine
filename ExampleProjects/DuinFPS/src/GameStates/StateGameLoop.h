#pragma once

#include <Duin.h>

class StateGameLoop : public Duin::GameState
{
public:
   StateGameLoop(Duin::GameStateMachine& owner);
    ~StateGameLoop() override;

private:
    void State_Enter();
    void State_Exit();
    void State_HandleInput();
    void State_Update(double delta);
    void State_PhysicsUpdate(double delta);
    void State_Draw();
    void State_DrawUI();
};
