#include <doctest.h>
#include <Duin/Core/Utils/StateMachine.h>
#include <Duin/Core/Events/Event.h>
#include <string>
#include <vector>

// Test states for testing StateMachine behavior

class TestStateA : public duin::State
{
  public:
    TestStateA(duin::StateMachine &owner) : duin::State(owner)
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

class TestStateB : public duin::State
{
  public:
    TestStateB(duin::StateMachine &owner) : duin::State(owner)
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

class TestStateC : public duin::State
{
  public:
    TestStateC(duin::StateMachine &owner) : duin::State(owner)
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
class SwitchingState : public duin::State
{
  public:
    SwitchingState(duin::StateMachine &owner) : duin::State(owner)
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


namespace TestStateMachine
{

TEST_SUITE("StateMachine")
{
    TEST_CASE("StateMachine UUID is valid and unique")
    {
        duin::StateMachine sm1;
        duin::StateMachine sm2;

        CHECK(sm1.GetUUID() != duin::UUID::INVALID);
        CHECK(sm2.GetUUID() != duin::UUID::INVALID);
        CHECK(sm1.GetUUID() != sm2.GetUUID());
    }

    TEST_CASE("PushState adds state and calls Enter")
    {
        duin::StateMachine sm;

        sm.PushState<TestStateA>();

        auto *state = sm.GetActiveState<TestStateA>();
        CHECK(state != nullptr);
        CHECK(state->enterCalled);
        CHECK_FALSE(state->exitCalled);
    }

    TEST_CASE("PushState multiple states creates stack")
    {
        duin::StateMachine sm;

        sm.PushState<TestStateA>();
        sm.PushState<TestStateB>();
        sm.PushState<TestStateC>();

        auto *topState = sm.GetActiveState<TestStateC>();
        CHECK(topState != nullptr);
        CHECK(topState->enterCalled);
    }

    TEST_CASE("GetActiveState returns nullptr when stack is empty")
    {
        duin::StateMachine sm;

        auto *state = sm.GetActiveState<TestStateA>();
        CHECK(state == nullptr);
    }

    TEST_CASE("GetActiveState returns top of stack")
    {
        duin::StateMachine sm;

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
        duin::StateMachine sm;

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
        duin::StateMachine sm;

        // Should not crash
        sm.PopState();
        sm.PopState();

        auto *state = sm.GetActiveState<TestStateA>();
        CHECK(state == nullptr);
    }

    TEST_CASE("SwitchState replaces top state")
    {
        duin::StateMachine sm;

        sm.PushState<TestStateA>();
        auto *stateA = sm.GetActiveState<TestStateA>();
        CHECK(stateA != nullptr);
        CHECK_FALSE(stateA->exitCalled);

        sm.SwitchState<TestStateB>();

        CHECK(stateA->exitCalled);

        auto *stateB = sm.GetActiveState<TestStateB>();
        CHECK(stateB != nullptr);
        CHECK(stateB->enterCalled);
    }

    TEST_CASE("SwitchState on empty stack pushes new state")
    {
        duin::StateMachine sm;

        sm.SwitchState<TestStateA>();

        auto *state = sm.GetActiveState<TestStateA>();
        CHECK(state != nullptr);
        CHECK(state->enterCalled);
    }

    TEST_CASE("FlushAndSwitchState clears all states")
    {
        duin::StateMachine sm;

        auto *stateA = sm.PushState<TestStateA>();
        auto *stateB = sm.PushState<TestStateB>();
        auto *stateC = sm.PushState<TestStateC>();

        sm.FlushAndSwitchState<TestStateA>();

        // All previous states should have Exit called
        CHECK(stateA->exitCalled);
        CHECK(stateB->exitCalled);
        CHECK(stateC->exitCalled);

        // New TestStateA should be on top
        auto *newState = sm.GetActiveState<TestStateA>();
        CHECK(newState != nullptr);
        CHECK(newState->enterCalled);
        CHECK(newState != stateA); // Should be a different instance
    }

    TEST_CASE("FlushStack removes all states")
    {
        duin::StateMachine sm;

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
        duin::StateMachine sm;

        // Should not crash
        sm.FlushStack();

        auto *state = sm.GetActiveState<TestStateA>();
        CHECK(state == nullptr);
    }

    TEST_CASE("ExecuteUpdate calls top state Update")
    {
        duin::StateMachine sm;
        sm.PushState<TestStateA>();

        auto *state = sm.GetActiveState<TestStateA>();
        CHECK(state->updateCalls == 0);

        sm.ExecuteUpdate(0.016);
        CHECK(state->updateCalls == 1);
        CHECK(state->lastDelta == doctest::Approx(0.016));

        sm.ExecuteUpdate(0.033);
        CHECK(state->updateCalls == 2);
        CHECK(state->lastDelta == doctest::Approx(0.033));
    }

    TEST_CASE("ExecuteUpdate on empty stack does nothing")
    {
        duin::StateMachine sm;

        // Should not crash
        sm.ExecuteUpdate(0.016);
        CHECK(true);
    }

    TEST_CASE("ExecutePhysicsUpdate calls top state PhysicsUpdate")
    {
        duin::StateMachine sm;
        sm.PushState<TestStateA>();

        auto *state = sm.GetActiveState<TestStateA>();
        CHECK(state->physicsUpdateCalls == 0);

        sm.ExecutePhysicsUpdate(0.016);
        CHECK(state->physicsUpdateCalls == 1);

        sm.ExecutePhysicsUpdate(0.016);
        CHECK(state->physicsUpdateCalls == 2);
    }

    TEST_CASE("ExecuteDraw calls top state Draw")
    {
        duin::StateMachine sm;
        sm.PushState<TestStateA>();

        auto *state = sm.GetActiveState<TestStateA>();
        CHECK(state->drawCalls == 0);

        sm.ExecuteDraw();
        CHECK(state->drawCalls == 1);

        sm.ExecuteDraw();
        CHECK(state->drawCalls == 2);
    }

    TEST_CASE("ExecuteDrawUI calls top state DrawUI")
    {
        duin::StateMachine sm;
        sm.PushState<TestStateA>();

        auto *state = sm.GetActiveState<TestStateA>();
        CHECK(state->drawUICalls == 0);

        sm.ExecuteDrawUI();
        CHECK(state->drawUICalls == 1);

        sm.ExecuteDrawUI();
        CHECK(state->drawUICalls == 2);
    }

    TEST_CASE("ExecuteOnEvent calls top state OnEvent")
    {
        duin::StateMachine sm;
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

        sm.ExecuteOnEvent(testEvent);
        CHECK(state->eventCalls == 1);
    }

    TEST_CASE("State UUID is valid and unique")
    {
        duin::StateMachine sm;
        sm.PushState<TestStateA>();
        auto *stateA = sm.GetActiveState<TestStateA>();

        sm.PushState<TestStateB>();
        auto *stateB = sm.GetActiveState<TestStateB>();

        CHECK(stateA->GetUUID() != duin::UUID::INVALID);
        CHECK(stateB->GetUUID() != duin::UUID::INVALID);
        CHECK(stateA->GetUUID() != stateB->GetUUID());
    }

    TEST_CASE("State GetOwner returns StateMachine")
    {
        duin::StateMachine sm;
        sm.PushState<TestStateA>();

        auto *state = sm.GetActiveState<TestStateA>();
        CHECK(&state->GetOwner() == &sm);
    }

    TEST_CASE("State GetName returns state name")
    {
        duin::StateMachine sm;
        sm.PushState<TestStateA>();

        auto *state = sm.GetActiveState<TestStateA>();
        CHECK(state->GetName() == "TestStateA");
    }

    TEST_CASE("State equality operators")
    {
        duin::StateMachine sm;
        sm.PushState<TestStateA>();
        auto *stateA1 = sm.GetActiveState<TestStateA>();

        sm.PushState<TestStateA>();
        auto *stateA2 = sm.GetActiveState<TestStateA>();

        // Different instances should not be equal
        CHECK(*stateA1 != *stateA2);

        // Same instance should be equal to itself
        CHECK(*stateA1 == *stateA1);
    }

    TEST_CASE("State can call PopState on owner")
    {
        duin::StateMachine sm;
        sm.PushState<TestStateA>();
        sm.PushState<TestStateB>();

        auto *stateB = sm.GetActiveState<TestStateB>();
        CHECK(stateB != nullptr);

        stateB->PopState();

        // TestStateA should now be on top
        auto *stateA = sm.GetActiveState<TestStateA>();
        CHECK(stateA != nullptr);
    }

    TEST_CASE("State can call SwitchState on owner")
    {
        duin::StateMachine sm;
        auto *stateA = sm.PushState<TestStateA>();

        CHECK(stateA != nullptr);

        auto *stateB = stateA->SwitchState<TestStateB>();

        CHECK(stateB != nullptr);
        CHECK(stateA->exitCalled);
    }

    TEST_CASE("State can call FlushAndSwitchState on owner")
    {
        duin::StateMachine sm;
        sm.PushState<TestStateA>();
        sm.PushState<TestStateB>();
        sm.PushState<TestStateC>();

        auto *stateC = sm.GetActiveState<TestStateC>();
        stateC->FlushAndSwitchState<TestStateA>();

        auto *newState = sm.GetActiveState<TestStateA>();
        CHECK(newState != nullptr);
        CHECK(newState->enterCalled);
    }

    TEST_CASE("State can call PushState on owner")
    {
        duin::StateMachine sm;
        sm.PushState<TestStateA>();

        auto *stateA = sm.GetActiveState<TestStateA>();
        stateA->PushState<TestStateB>();

        auto *stateB = sm.GetActiveState<TestStateB>();
        CHECK(stateB != nullptr);
        CHECK(stateB->enterCalled);
    }

    TEST_CASE("State can call FlushStack on owner")
    {
        duin::StateMachine sm;
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
        duin::StateMachine sm;

        // Push A, B, C
        sm.PushState<TestStateA>();
        sm.PushState<TestStateB>();
        sm.PushState<TestStateC>();

        // Pop C, should have B on top
        sm.PopState();
        auto *stateB = sm.GetActiveState<TestStateB>();
        CHECK(stateB != nullptr);

        // Switch B to A, should have new A on top
        sm.SwitchState<TestStateA>();
        auto *newStateA = sm.GetActiveState<TestStateA>();
        CHECK(newStateA != nullptr);
        CHECK(stateB->exitCalled);

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
} // namespace TestStateMachine
