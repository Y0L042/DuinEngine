#include "CLASS_NAME.h"

CLASS_NAME::CLASS_NAME(duin::GameStateMachine& owner)
	: GameState(owner)
{
    Enter = [this]() { State_Enter(); };
    Exit = [this]() { State_Exit(); };
    HandleInput = [this]() { State_HandleInput(); };
    Update = [this](double delta) { State_Update(delta); };
    PhysicsUpdate = [this](double delta) { State_PhysicsUpdate(delta); };
    Draw = [this]() { State_Draw(); };
    DrawUI = [this]() { State_DrawUI(); };
}

void CLASS_NAME::State_Enter()
{
}

void CLASS_NAME::State_HandleInput()
{
}

void CLASS_NAME::State_Update(double delta)
{
}

void CLASS_NAME::State_PhysicsUpdate(double delta)
{
}

void CLASS_NAME::State_Draw()
{
}

void CLASS_NAME::State_DrawUI()
{
}

void CLASS_NAME::State_Exit()
{
}
