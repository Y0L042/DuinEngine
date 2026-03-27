#pragma once

#include <Duin/Objects/GameStateMachine.h>
#include <Duin/Core/Events/EventsModule.h>
#include "PlayerStateMachine.h"
#include "PlayerBlackboard.h"

class PlayerState : public duin::GameState
{
  public:
    using duin::GameState::GameState;

  protected:
    PlayerBlackboard* GetBlackboard()
    {
        return static_cast<PlayerStateMachine*>(&owner)->GetBlackboard();
    }

    duin::Entity& GetPlayer()
    {
        return GetBlackboard()->player;
    }

    duin::GameWorld* GetWorld()
    {
        return GetBlackboard()->world;
    }

    static bool IsMovementInputPressed()
    {
        return duin::Input::IsInputVectorPressed(DN_SCANCODE_W, DN_SCANCODE_S, DN_SCANCODE_A, DN_SCANCODE_D);
    }

    static duin::Vector2 GetMovementInput()
    {
        return duin::Input::GetInputVector(DN_SCANCODE_W, DN_SCANCODE_S, DN_SCANCODE_A, DN_SCANCODE_D);
    }
};
