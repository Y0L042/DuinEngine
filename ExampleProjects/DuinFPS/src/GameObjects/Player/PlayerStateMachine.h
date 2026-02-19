#pragma once

#include <Duin/Objects/GameStateMachine.h>
#include "PlayerBlackboard.h"

class PlayerStateMachine : public duin::GameStateMachine
{
  public:
    PlayerStateMachine(PlayerBlackboard* blackboard) 
        : duin::GameStateMachine(), blackboard(blackboard) 
    {
    }
    
    PlayerBlackboard* GetBlackboard() { return blackboard; }
    
  private:
    PlayerBlackboard* blackboard;
};
