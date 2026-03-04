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

// State that calls SwitchState/PopState/FlushStack from inside each dispatch method.
// The action is controlled by the action* flags so each dispatch path can be tested
// independently.
class SelfDestructingState : public duin::GameState
{
  public:
    SelfDestructingState(duin::GameStateMachine &owner) : duin::GameState(owner)
    {
        stateName = "SelfDestructingState";
    }

    enum class Action { None, Switch, Pop, Flush };

    void DrawUI() override
    {
        drawUICalled = true;
        if (drawUIAction == Action::Switch)  SwitchState<TestStateB>();
        else if (drawUIAction == Action::Pop)   PopState();
        else if (drawUIAction == Action::Flush) FlushStack();
    }

    void Update(double) override
    {
        updateCalled = true;
        if (updateAction == Action::Switch)  SwitchState<TestStateB>();
        else if (updateAction == Action::Pop)   PopState();
        else if (updateAction == Action::Flush) FlushStack();
    }

    void PhysicsUpdate(double) override
    {
        physicsUpdateCalled = true;
        if (physicsUpdateAction == Action::Switch)  SwitchState<TestStateB>();
        else if (physicsUpdateAction == Action::Pop)   PopState();
        else if (physicsUpdateAction == Action::Flush) FlushStack();
    }

    void Draw() override
    {
        drawCalled = true;
        if (drawAction == Action::Switch)  SwitchState<TestStateB>();
        else if (drawAction == Action::Pop)   PopState();
        else if (drawAction == Action::Flush) FlushStack();
    }

    void OnEvent(duin::Event) override
    {
        onEventCalled = true;
        if (onEventAction == Action::Switch)  SwitchState<TestStateB>();
        else if (onEventAction == Action::Pop)   PopState();
        else if (onEventAction == Action::Flush) FlushStack();
    }

    bool drawUICalled        = false;
    bool updateCalled        = false;
    bool physicsUpdateCalled = false;
    bool drawCalled          = false;
    bool onEventCalled       = false;

    Action drawUIAction        = Action::None;
    Action updateAction        = Action::None;
    Action physicsUpdateAction = Action::None;
    Action drawAction          = Action::None;
    Action onEventAction       = Action::None;
};
