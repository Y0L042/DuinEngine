#pragma once

#include <Duin/Core/Utils/UtilsModule.h>

class PlayerStateOnGroundSprint : public duin::GameState
{
public:
    PlayerStateOnGroundSprint(duin::GameStateMachine& owner);

private:
    void State_Enter();
    void State_HandleInput();
    void State_Update(double delta);
    void State_PhysicsUpdate(double delta);
    void State_Draw();
    void State_DrawUI();
    void State_Exit();

};

