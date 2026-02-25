#pragma once
#include <doctest.h>
#include "Defines.h"
#include <Duin/Objects/GameStateMachine.h>
#include <Duin/Core/Events/Event.h>
#include <string>
#include <vector>

// Test states for testing StateMachine behavior

class TestStateA : public duin::GameState
{
  public:
    TestStateA(duin::GameStateMachine &owner) : duin::GameState(owner)
    {
        stateName = "TestStateA";
    }

    void Enter() override
    {
        enterCalled = true;
    }
    void Exit() override
    {
        exitCalled = true;
    }
    void Update(double delta) override
    {
        updateCalls++;
        lastDelta = delta;
    }
    void PhysicsUpdate(double delta) override
    {
        physicsUpdateCalls++;
    }
    void Draw() override
    {
        drawCalls++;
    }
    void DrawUI() override
    {
        drawUICalls++;
    }
    void OnEvent(duin::Event e) override
    {
        eventCalls++;
        lastEvent = e;
    }

    bool enterCalled = false;
    bool exitCalled = false;
    int updateCalls = 0;
    int physicsUpdateCalls = 0;
    int drawCalls = 0;
    int drawUICalls = 0;
    int eventCalls = 0;
    double lastDelta = 0.0;
    duin::Event lastEvent;
};

class TestStateB : public duin::GameState
{
  public:
    TestStateB(duin::GameStateMachine &owner) : duin::GameState(owner)
    {
        stateName = "TestStateB";
    }

    void Enter() override
    {
        enterCalled = true;
    }
    void Exit() override
    {
        exitCalled = true;
    }

    bool enterCalled = false;
    bool exitCalled = false;
};

class TestStateC : public duin::GameState
{
  public:
    TestStateC(duin::GameStateMachine &owner) : duin::GameState(owner)
    {
        stateName = "TestStateC";
    }

    void Enter() override
    {
        enterCalled = true;
    }
    void Exit() override
    {
        exitCalled = true;
    }

    bool enterCalled = false;
    bool exitCalled = false;
};

// Simple GameObject that counts lifecycle callback invocations
class TrackingObject : public duin::GameObject
{
  public:
    void Update(double delta) override
    {
        updateCalls++;
    }
    void PhysicsUpdate(double delta) override
    {
        physicsUpdateCalls++;
    }
    void Draw() override
    {
        drawCalls++;
    }
    void DrawUI() override
    {
        drawUICalls++;
    }
    void OnEvent(duin::Event e) override
    {
        eventCalls++;
    }

    int updateCalls = 0;
    int physicsUpdateCalls = 0;
    int drawCalls = 0;
    int drawUICalls = 0;
    int eventCalls = 0;
};

// State that switches to another state in Enter
class SwitchingState : public duin::GameState
{
  public:
    SwitchingState(duin::GameStateMachine &owner) : duin::GameState(owner)
    {
    }

    void Enter() override
    {
        enterCalled = true;
        if (shouldSwitch)
            SwitchState<TestStateB>();
    }

    bool enterCalled = false;
    bool shouldSwitch = false;
};
