#pragma once

#include <Duin.h>

extern flecs::entity player;

class StateGameLoop : public duin::GameState
{
public:
   StateGameLoop(duin::GameStateMachine& owner);
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
