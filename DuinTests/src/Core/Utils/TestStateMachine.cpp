#include <doctest.h>
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

namespace TestGameStateMachine
{

TEST_SUITE("GameStateMachine")
{
    TEST_CASE("GameStateMachine UUID is valid and unique")
    {
        duin::GameStateMachine sm1;
        duin::GameStateMachine sm2;

        CHECK(sm1.GetUUID() != duin::UUID::INVALID);
        CHECK(sm2.GetUUID() != duin::UUID::INVALID);
        CHECK(sm1.GetUUID() != sm2.GetUUID());
    }

    TEST_CASE("PushState adds state and calls Enter")
    {
        duin::GameStateMachine sm;

        sm.PushState<TestStateA>();

        auto *state = sm.GetActiveState<TestStateA>();
        CHECK(state != nullptr);
        CHECK(state->enterCalled);
        CHECK_FALSE(state->exitCalled);
    }

    TEST_CASE("PushState multiple states creates stack")
    {
        duin::GameStateMachine sm;

        sm.PushState<TestStateA>();
        sm.PushState<TestStateB>();
        sm.PushState<TestStateC>();

        auto *topState = sm.GetActiveState<TestStateC>();
        CHECK(topState != nullptr);
        CHECK(topState->enterCalled);
    }

    TEST_CASE("GetActiveState returns nullptr when stack is empty")
    {
        duin::GameStateMachine sm;

        auto *state = sm.GetActiveState<TestStateA>();
        CHECK(state == nullptr);
    }

    TEST_CASE("GetActiveState returns top of stack")
    {
        duin::GameStateMachine sm;

        sm.PushState<TestStateA>();
        sm.PushState<TestStateB>();

        auto *stateB = sm.GetActiveState<TestStateB>();
        CHECK(stateB != nullptr);

        // Cannot get TestStateA as it's not on top
        auto *stateA = sm.GetActiveState<TestStateA>();
        CHECK(stateA == nullptr);
    }

    TEST_CASE("PopState removes top state and calls Exit")
    {
        duin::GameStateMachine sm;

        sm.PushState<TestStateA>();
        sm.PushState<TestStateB>();

        auto *stateB = sm.GetActiveState<TestStateB>();
        CHECK(stateB != nullptr);
        CHECK_FALSE(stateB->exitCalled);

        sm.PopState();

        CHECK(stateB->exitCalled);

        // Now TestStateA should be on top
        auto *stateA = sm.GetActiveState<TestStateA>();
        CHECK(stateA != nullptr);
    }

    TEST_CASE("PopState on empty stack does nothing")
    {
        duin::GameStateMachine sm;

        // Should not crash
        sm.PopState();
        sm.PopState();

        auto *state = sm.GetActiveState<TestStateA>();
        CHECK(state == nullptr);
    }

    TEST_CASE("SwitchState replaces top state")
    {
        duin::GameStateMachine sm;

        sm.PushState<TestStateA>();
        auto *stateA = sm.GetActiveState<TestStateA>();
        CHECK(stateA != nullptr);
        CHECK_FALSE(stateA->exitCalled);

        // Use signal to verify Exit is called before state is destroyed
        bool exitCalled = false;
        stateA->ConnectOnStateExit([&exitCalled]() { exitCalled = true; });

        sm.SwitchState<TestStateB>();

        CHECK(exitCalled);

        auto *stateB = sm.GetActiveState<TestStateB>();
        CHECK(stateB != nullptr);
        CHECK(stateB->enterCalled);
    }

    TEST_CASE("SwitchState on empty stack pushes new state")
    {
        duin::GameStateMachine sm;

        sm.SwitchState<TestStateA>();

        auto *state = sm.GetActiveState<TestStateA>();
        CHECK(state != nullptr);
        CHECK(state->enterCalled);
    }

    TEST_CASE("FlushAndSwitchState clears all states")
    {
        duin::GameStateMachine sm;

        auto *stateA = sm.PushState<TestStateA>();
        auto *stateB = sm.PushState<TestStateB>();
        auto *stateC = sm.PushState<TestStateC>();

        // Use signals to verify Exit is called before states are destroyed
        bool exitCalledA = false;
        bool exitCalledB = false;
        bool exitCalledC = false;
        stateA->ConnectOnStateExit([&exitCalledA]() { exitCalledA = true; });
        stateB->ConnectOnStateExit([&exitCalledB]() { exitCalledB = true; });
        stateC->ConnectOnStateExit([&exitCalledC]() { exitCalledC = true; });

        // Store UUID to verify new instance
        auto oldStateA = stateA;
        duin::UUID oldStateAUUID = stateA->GetUUID();

        sm.FlushAndSwitchState<TestStateA>();

        // All previous states should have Exit called
        CHECK(exitCalledA);
        CHECK(exitCalledB);
        CHECK(exitCalledC);

        // New TestStateA should be on top
        auto *newState = sm.GetActiveState<TestStateA>();
        CHECK(newState != nullptr);
        CHECK(newState->enterCalled);
        //CHECK(newState->GetUUID() != oldStateAUUID);
        CHECK(!newState->IsEqualTo(oldState))
    }

    TEST_CASE("FlushStack removes all states")
    {
        duin::GameStateMachine sm;

        sm.PushState<TestStateA>();
        auto *stateA = sm.GetActiveState<TestStateA>();
        sm.PushState<TestStateB>();
        auto *stateB = sm.GetActiveState<TestStateB>();

        sm.FlushStack();

        CHECK(stateA->exitCalled);
        CHECK(stateB->exitCalled);

        auto *state = sm.GetActiveState<TestStateA>();
        CHECK(state == nullptr);
    }

    TEST_CASE("FlushStack on empty stack does nothing")
    {
        duin::GameStateMachine sm;

        // Should not crash
        sm.FlushStack();

        auto *state = sm.GetActiveState<TestStateA>();
        CHECK(state == nullptr);
    }

    TEST_CASE("Update calls top state Update")
    {
        duin::GameStateMachine sm;
        sm.PushState<TestStateA>();

        auto *state = sm.GetActiveState<TestStateA>();
        CHECK(state->updateCalls == 0);

        sm.Update(0.016);
        CHECK(state->updateCalls == 1);
        CHECK(state->lastDelta == doctest::Approx(0.016));

        sm.Update(0.033);
        CHECK(state->updateCalls == 2);
        CHECK(state->lastDelta == doctest::Approx(0.033));
    }

    TEST_CASE("Update on empty stack does nothing")
    {
        duin::GameStateMachine sm;

        // Should not crash
        sm.Update(0.016);
        CHECK(true);
    }

    TEST_CASE("PhysicsUpdate calls top state PhysicsUpdate")
    {
        duin::GameStateMachine sm;
        sm.PushState<TestStateA>();

        auto *state = sm.GetActiveState<TestStateA>();
        CHECK(state->physicsUpdateCalls == 0);

        sm.PhysicsUpdate(0.016);
        CHECK(state->physicsUpdateCalls == 1);

        sm.PhysicsUpdate(0.016);
        CHECK(state->physicsUpdateCalls == 2);
    }

    TEST_CASE("Draw calls top state Draw")
    {
        duin::GameStateMachine sm;
        sm.PushState<TestStateA>();

        auto *state = sm.GetActiveState<TestStateA>();
        CHECK(state->drawCalls == 0);

        sm.Draw();
        CHECK(state->drawCalls == 1);

        sm.Draw();
        CHECK(state->drawCalls == 2);
    }

    TEST_CASE("DrawUI calls top state DrawUI")
    {
        duin::GameStateMachine sm;
        sm.PushState<TestStateA>();

        auto *state = sm.GetActiveState<TestStateA>();
        CHECK(state->drawUICalls == 0);

        sm.DrawUI();
        CHECK(state->drawUICalls == 1);

        sm.DrawUI();
        CHECK(state->drawUICalls == 2);
    }

    TEST_CASE("OnEvent calls top state OnEvent")
    {
        duin::GameStateMachine sm;
        sm.PushState<TestStateA>();

        auto *state = sm.GetActiveState<TestStateA>();
        CHECK(state->eventCalls == 0);

        duin::Event testEvent{};
        SDL_Event user_event;
        SDL_zero(user_event);
        user_event.type = SDL_EVENT_USER;
        user_event.user.code = 2;
        user_event.user.data1 = NULL;
        user_event.user.data2 = NULL;
        testEvent.sdlEvent = user_event;

        sm.OnEvent(testEvent);
        CHECK(state->eventCalls == 1);
    }

    TEST_CASE("GameState UUID is valid and unique")
    {
        duin::GameStateMachine sm;
        sm.PushState<TestStateA>();
        auto *stateA = sm.GetActiveState<TestStateA>();

        sm.PushState<TestStateB>();
        auto *stateB = sm.GetActiveState<TestStateB>();

        CHECK(stateA->GetUUID() != duin::UUID::INVALID);
        CHECK(stateB->GetUUID() != duin::UUID::INVALID);
        CHECK(stateA->GetUUID() != stateB->GetUUID());
    }

    TEST_CASE("GameState GetOwner returns GameStateMachine")
    {
        duin::GameStateMachine sm;
        sm.PushState<TestStateA>();

        auto *state = sm.GetActiveState<TestStateA>();
        CHECK(&state->GetOwner() == &sm);
    }

    TEST_CASE("GameState GetName returns state name")
    {
        duin::GameStateMachine sm;
        sm.PushState<TestStateA>();

        auto *state = sm.GetActiveState<TestStateA>();
        CHECK(state->GetName() == "TestStateA");
    }

    TEST_CASE("GameState equality operators")
    {
        duin::GameStateMachine sm;
        sm.PushState<TestStateA>();
        auto *stateA1 = sm.GetActiveState<TestStateA>();

        sm.PushState<TestStateA>();
        auto *stateA2 = sm.GetActiveState<TestStateA>();

        // Different instances should not be equal
        CHECK(*stateA1 != *stateA2);

        // Same instance should be equal to itself
        CHECK(*stateA1 == *stateA1);
    }

    TEST_CASE("GameState can call PopState on owner")
    {
        duin::GameStateMachine sm;
        sm.PushState<TestStateA>();
        sm.PushState<TestStateB>();

        auto *stateB = sm.GetActiveState<TestStateB>();
        CHECK(stateB != nullptr);

        stateB->PopState();

        // TestStateA should now be on top
        auto *stateA = sm.GetActiveState<TestStateA>();
        CHECK(stateA != nullptr);
    }

    TEST_CASE("GameState can call SwitchState on owner")
    {
        duin::GameStateMachine sm;
        auto *stateA = sm.PushState<TestStateA>();

        CHECK(stateA != nullptr);

        // Use signal to verify Exit is called before state is destroyed
        bool exitCalled = false;
        stateA->ConnectOnStateExit([&exitCalled]() { exitCalled = true; });

        auto *stateB = stateA->SwitchState<TestStateB>();

        CHECK(stateB != nullptr);
        CHECK(exitCalled);
    }

    TEST_CASE("GameState can call FlushAndSwitchState on owner")
    {
        duin::GameStateMachine sm;
        sm.PushState<TestStateA>();
        sm.PushState<TestStateB>();
        sm.PushState<TestStateC>();

        auto *stateC = sm.GetActiveState<TestStateC>();
        stateC->FlushAndSwitchState<TestStateA>();

        auto *newState = sm.GetActiveState<TestStateA>();
        CHECK(newState != nullptr);
        CHECK(newState->enterCalled);
    }

    TEST_CASE("GameState can call PushState on owner")
    {
        duin::GameStateMachine sm;
        sm.PushState<TestStateA>();

        auto *stateA = sm.GetActiveState<TestStateA>();
        stateA->PushState<TestStateB>();

        auto *stateB = sm.GetActiveState<TestStateB>();
        CHECK(stateB != nullptr);
        CHECK(stateB->enterCalled);
    }

    TEST_CASE("GameState can call FlushStack on owner")
    {
        duin::GameStateMachine sm;
        sm.PushState<TestStateA>();
        auto *stateA = sm.GetActiveState<TestStateA>();
        sm.PushState<TestStateB>();

        auto *stateB = sm.GetActiveState<TestStateB>();
        stateB->FlushStack();

        auto *state = sm.GetActiveState<TestStateA>();
        CHECK(state == nullptr);
    }

    TEST_CASE("Complex state transitions")
    {
        duin::GameStateMachine sm;

        // Push A, B, C
        sm.PushState<TestStateA>();
        sm.PushState<TestStateB>();
        sm.PushState<TestStateC>();

        // Pop C, should have B on top
        sm.PopState();
        auto *stateB = sm.GetActiveState<TestStateB>();
        CHECK(stateB != nullptr);

        // Use signal to verify Exit is called when B is switched
        bool stateBExitCalled = false;
        stateB->ConnectOnStateExit([&stateBExitCalled]() { stateBExitCalled = true; });

        // Switch B to A, should have new A on top
        sm.SwitchState<TestStateA>();
        auto *newStateA = sm.GetActiveState<TestStateA>();
        CHECK(newStateA != nullptr);
        CHECK(stateBExitCalled);

        // Push C on top of new A
        sm.PushState<TestStateC>();
        auto *stateC = sm.GetActiveState<TestStateC>();
        CHECK(stateC != nullptr);

        // Flush and switch to B
        sm.FlushAndSwitchState<TestStateB>();
        auto *finalState = sm.GetActiveState<TestStateB>();
        CHECK(finalState != nullptr);
        CHECK(finalState->enterCalled);
    }
}
} // namespace TestGameStateMachine
