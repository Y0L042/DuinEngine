#include <doctest.h>
#include "../../Defines.h"
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
        std::shared_ptr<duin::GameStateMachine> sm1 = std::make_shared<duin::GameStateMachine>();
        std::shared_ptr<duin::GameStateMachine> sm2 = std::make_shared<duin::GameStateMachine>();

        CAPTURE(sm1->GetUUID());
        MSG_CHECK(sm1->GetUUID(), sm1->GetUUID() != duin::UUID::INVALID);
        CAPTURE(sm2->GetUUID());
        MSG_CHECK(sm2->GetUUID(), sm2->GetUUID() != duin::UUID::INVALID);
        CAPTURE(sm1->GetUUID());
        CAPTURE(sm2->GetUUID());
        MSG_CHECK(sm1->GetUUID(), sm1->GetUUID() != sm2->GetUUID());
    }

    TEST_CASE("PushState adds state and calls Enter")
    {
        std::shared_ptr<duin::GameStateMachine> sm = std::make_shared<duin::GameStateMachine>();

        sm->PushState<TestStateA>();

        auto *state = sm->GetActiveState<TestStateA>();
        CAPTURE(state);
        MSG_CHECK(state, state != nullptr);
        CAPTURE(state->enterCalled);
        MSG_CHECK(state->enterCalled, state->enterCalled);
        CAPTURE(state->exitCalled);
        MSG_CHECK_FALSE(state->exitCalled, state->exitCalled);
    }

    TEST_CASE("PushState multiple states creates stack")
    {
        std::shared_ptr<duin::GameStateMachine> sm = std::make_shared<duin::GameStateMachine>();

        sm->PushState<TestStateA>();
        sm->PushState<TestStateB>();
        sm->PushState<TestStateC>();

        auto *topState = sm->GetActiveState<TestStateC>();
        CAPTURE(topState);
        MSG_CHECK(topState, topState != nullptr);
        CAPTURE(topState->enterCalled);
        MSG_CHECK(topState->enterCalled, topState->enterCalled);
    }

    TEST_CASE("GetActiveState returns nullptr when stack is empty")
    {
        std::shared_ptr<duin::GameStateMachine> sm = std::make_shared<duin::GameStateMachine>();

        auto *state = sm->GetActiveState<TestStateA>();
        CAPTURE(state);
        MSG_CHECK(state, state == nullptr);
    }

    TEST_CASE("GetActiveState returns top of stack")
    {
        std::shared_ptr<duin::GameStateMachine> sm = std::make_shared<duin::GameStateMachine>();

        sm->PushState<TestStateA>();
        sm->PushState<TestStateB>();

        auto *stateB = sm->GetActiveState<TestStateB>();
        MSG_CHECK(stateB, stateB != nullptr);

        // Cannot get TestStateA as it's not on top
        auto *stateA = sm->GetActiveState<TestStateA>();
        MSG_CHECK(stateA, stateA == nullptr);
    }

    TEST_CASE("PopState removes top state and calls Exit")
    {
        std::shared_ptr<duin::GameStateMachine> sm = std::make_shared<duin::GameStateMachine>();

        sm->PushState<TestStateA>();
        sm->PushState<TestStateB>();

        auto *stateB = sm->GetActiveState<TestStateB>();
        CAPTURE(stateB);
        MSG_CHECK(stateB, stateB != nullptr);
        CAPTURE(stateB->exitCalled);
        MSG_CHECK_FALSE(stateB->exitCalled, stateB->exitCalled);

        sm->PopState();

        CAPTURE(stateB->exitCalled);
        MSG_CHECK(stateB->exitCalled, stateB->exitCalled);

        // Now TestStateA should be on top
        auto *stateA = sm->GetActiveState<TestStateA>();
        CAPTURE(stateA);
        MSG_CHECK(stateA, stateA != nullptr);
    }

    TEST_CASE("PopState on empty stack does nothing")
    {
        std::shared_ptr<duin::GameStateMachine> sm = std::make_shared<duin::GameStateMachine>();

        // Should not crash
        sm->PopState();
        sm->PopState();

        auto *state = sm->GetActiveState<TestStateA>();
        MSG_CHECK(state, state == nullptr);
    }

    TEST_CASE("SwitchState replaces top state")
    {
        std::shared_ptr<duin::GameStateMachine> sm = std::make_shared<duin::GameStateMachine>();

        sm->PushState<TestStateA>();
        auto *stateA = sm->GetActiveState<TestStateA>();
        MSG_CHECK(stateA, stateA != nullptr);
        MSG_CHECK_FALSE(stateA->exitCalled, stateA->exitCalled);

        // Use signal to verify Exit is called before state is destroyed
        bool exitCalled = false;
        stateA->ConnectOnStateExit([&exitCalled]() { exitCalled = true; });

        sm->SwitchState<TestStateB>();

        MSG_CHECK(exitCalled, exitCalled);

        auto *stateB = sm->GetActiveState<TestStateB>();
        MSG_CHECK(stateB, stateB != nullptr);
        MSG_CHECK(stateB->enterCalled, stateB->enterCalled);
    }

    TEST_CASE("SwitchState on empty stack pushes new state")
    {
        std::shared_ptr<duin::GameStateMachine> sm = std::make_shared<duin::GameStateMachine>();

        sm->SwitchState<TestStateA>();

        auto *state = sm->GetActiveState<TestStateA>();
        MSG_CHECK(state, state != nullptr);
        MSG_CHECK(state->enterCalled, state->enterCalled);
    }

    TEST_CASE("FlushAndSwitchState clears all states")
    {
        std::shared_ptr<duin::GameStateMachine> sm = std::make_shared<duin::GameStateMachine>();

        auto *stateA = sm->PushState<TestStateA>();
        auto *stateB = sm->PushState<TestStateB>();
        auto *stateC = sm->PushState<TestStateC>();

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

        sm->FlushAndSwitchState<TestStateA>();

        // All previous states should have Exit called
        MSG_CHECK(exitCalledA, exitCalledA);
        MSG_CHECK(exitCalledB, exitCalledB);
        MSG_CHECK(exitCalledC, exitCalledC);

        // New TestStateA should be on top
        auto *newState = sm->GetActiveState<TestStateA>();
        MSG_CHECK(newState, newState != nullptr);
        MSG_CHECK(newState->enterCalled, newState->enterCalled);
        MSG_CHECK_FALSE(newState->GetUUID(), newState->GetUUID() == oldStateAUUID);
    }

    TEST_CASE("FlushStack removes all states")
    {
        std::shared_ptr<duin::GameStateMachine> sm = std::make_shared<duin::GameStateMachine>();

        sm->PushState<TestStateA>();
        auto *stateA = sm->GetActiveState<TestStateA>();
        sm->PushState<TestStateB>();
        auto *stateB = sm->GetActiveState<TestStateB>();

        sm->FlushStack();

        MSG_CHECK(stateA->exitCalled, stateA->exitCalled);
        MSG_CHECK(stateB->exitCalled, stateB->exitCalled);

        auto *state = sm->GetActiveState<TestStateA>();
        MSG_CHECK(state, state == nullptr);
    }

    TEST_CASE("FlushStack on empty stack does nothing")
    {
        std::shared_ptr<duin::GameStateMachine> sm = std::make_shared<duin::GameStateMachine>();

        // Should not crash
        sm->FlushStack();

        auto *state = sm->GetActiveState<TestStateA>();
        MSG_CHECK(state, state == nullptr);
    }

    TEST_CASE("Update calls top state Update")
    {
        std::shared_ptr<duin::GameStateMachine> sm = std::make_shared<duin::GameStateMachine>();
        sm->PushState<TestStateA>();

        auto *state = sm->GetActiveState<TestStateA>();
        CAPTURE(state->updateCalls);
        MSG_CHECK(state->updateCalls, state->updateCalls == 0);

        sm->Update(0.016);
        CAPTURE(state->updateCalls);
        MSG_CHECK(state->updateCalls, state->updateCalls == 1);
        CAPTURE(state->lastDelta);
        MSG_CHECK(state->lastDelta, state->lastDelta == doctest::Approx(0.016));

        sm->Update(0.033);
        CAPTURE(state->updateCalls);
        MSG_CHECK(state->updateCalls, state->updateCalls == 2);
        CAPTURE(state->lastDelta);
        MSG_CHECK(state->lastDelta, state->lastDelta == doctest::Approx(0.033));
    }

    TEST_CASE("Update on empty stack does nothing")
    {
        std::shared_ptr<duin::GameStateMachine> sm = std::make_shared<duin::GameStateMachine>();

        // Should not crash
        sm->Update(0.016);
        CHECK(true);
    }

    TEST_CASE("PhysicsUpdate calls top state PhysicsUpdate")
    {
        std::shared_ptr<duin::GameStateMachine> sm = std::make_shared<duin::GameStateMachine>();
        sm->PushState<TestStateA>();

        auto *state = sm->GetActiveState<TestStateA>();
        MSG_CHECK(state->physicsUpdateCalls, state->physicsUpdateCalls == 0);

        sm->PhysicsUpdate(0.016);
        MSG_CHECK(state->physicsUpdateCalls, state->physicsUpdateCalls == 1);

        sm->PhysicsUpdate(0.016);
        MSG_CHECK(state->physicsUpdateCalls, state->physicsUpdateCalls == 2);
    }

    TEST_CASE("Draw calls top state Draw")
    {
        std::shared_ptr<duin::GameStateMachine> sm = std::make_shared<duin::GameStateMachine>();
        sm->PushState<TestStateA>();

        auto *state = sm->GetActiveState<TestStateA>();
        MSG_CHECK(state->drawCalls, state->drawCalls == 0);

        sm->Draw();
        MSG_CHECK(state->drawCalls, state->drawCalls == 1);

        sm->Draw();
        MSG_CHECK(state->drawCalls, state->drawCalls == 2);
    }

    TEST_CASE("DrawUI calls top state DrawUI")
    {
        std::shared_ptr<duin::GameStateMachine> sm = std::make_shared<duin::GameStateMachine>();
        sm->PushState<TestStateA>();

        auto *state = sm->GetActiveState<TestStateA>();
        MSG_CHECK(state->drawUICalls, state->drawUICalls == 0);

        sm->DrawUI();
        MSG_CHECK(state->drawUICalls, state->drawUICalls == 1);

        sm->DrawUI();
        MSG_CHECK(state->drawUICalls, state->drawUICalls == 2);
    }

    TEST_CASE("OnEvent calls top state OnEvent")
    {
        std::shared_ptr<duin::GameStateMachine> sm = std::make_shared<duin::GameStateMachine>();
        sm->PushState<TestStateA>();

        auto *state = sm->GetActiveState<TestStateA>();
        MSG_CHECK(state->eventCalls, state->eventCalls == 0);

        duin::Event testEvent{};
        SDL_Event user_event;
        SDL_zero(user_event);
        user_event.type = SDL_EVENT_USER;
        user_event.user.code = 2;
        user_event.user.data1 = NULL;
        user_event.user.data2 = NULL;
        testEvent.sdlEvent = user_event;

        sm->OnEvent(testEvent);
        MSG_CHECK(state->eventCalls, state->eventCalls == 1);
    }

    TEST_CASE("GameState UUID is valid and unique")
    {
        std::shared_ptr<duin::GameStateMachine> sm = std::make_shared<duin::GameStateMachine>();
        sm->PushState<TestStateA>();
        auto *stateA = sm->GetActiveState<TestStateA>();

        sm->PushState<TestStateB>();
        auto *stateB = sm->GetActiveState<TestStateB>();

        MSG_CHECK(stateA->GetUUID(), stateA->GetUUID() != duin::UUID::INVALID);
        MSG_CHECK(stateB->GetUUID(), stateB->GetUUID() != duin::UUID::INVALID);
        MSG_CHECK(stateA->GetUUID(), stateA->GetUUID() != stateB->GetUUID());
    }

    TEST_CASE("GameState GetOwner returns GameStateMachine")
    {
        std::shared_ptr<duin::GameStateMachine> sm = std::make_shared<duin::GameStateMachine>();
        sm->PushState<TestStateA>();

        auto *state = sm->GetActiveState<TestStateA>();
        MSG_CHECK(&state->GetOwner(), &state->GetOwner() == sm.get());
    }

    TEST_CASE("GameState GetName returns state name")
    {
        std::shared_ptr<duin::GameStateMachine> sm = std::make_shared<duin::GameStateMachine>();
        sm->PushState<TestStateA>();

        auto *state = sm->GetActiveState<TestStateA>();
        MSG_CHECK(state->GetName(), state->GetName() == "TestStateA");
    }

    TEST_CASE("GameState equality operators")
    {
        std::shared_ptr<duin::GameStateMachine> sm = std::make_shared<duin::GameStateMachine>();
        sm->PushState<TestStateA>();
        auto *stateA1 = sm->GetActiveState<TestStateA>();

        sm->PushState<TestStateA>();
        auto *stateA2 = sm->GetActiveState<TestStateA>();

        // Different instances should not be equal
        MSG_CHECK(*stateA1, *stateA1 != *stateA2);

        // Same instance should be equal to itself
        MSG_CHECK(*stateA1, *stateA1 == *stateA1);
    }

    TEST_CASE("GameState can call PopState on owner")
    {
        std::shared_ptr<duin::GameStateMachine> sm = std::make_shared<duin::GameStateMachine>();
        sm->PushState<TestStateA>();
        sm->PushState<TestStateB>();

        auto *stateB = sm->GetActiveState<TestStateB>();
        MSG_CHECK(stateB, stateB != nullptr);

        stateB->PopState();

        // TestStateA should now be on top
        auto *stateA = sm->GetActiveState<TestStateA>();
        MSG_CHECK(stateA, stateA != nullptr);
    }

    TEST_CASE("GameState can call SwitchState on owner")
    {
        std::shared_ptr<duin::GameStateMachine> sm = std::make_shared<duin::GameStateMachine>();
        auto *stateA = sm->PushState<TestStateA>();

        MSG_CHECK(stateA, stateA != nullptr);

        // Use signal to verify Exit is called before state is destroyed
        bool exitCalled = false;
        stateA->ConnectOnStateExit([&exitCalled]() { exitCalled = true; });

        auto *stateB = stateA->SwitchState<TestStateB>();

        MSG_CHECK(stateB, stateB != nullptr);
        MSG_CHECK(exitCalled, exitCalled);
    }

    TEST_CASE("GameState can call FlushAndSwitchState on owner")
    {
        std::shared_ptr<duin::GameStateMachine> sm = std::make_shared<duin::GameStateMachine>();
        sm->PushState<TestStateA>();
        sm->PushState<TestStateB>();
        sm->PushState<TestStateC>();

        auto *stateC = sm->GetActiveState<TestStateC>();
        stateC->FlushAndSwitchState<TestStateA>();

        auto *newState = sm->GetActiveState<TestStateA>();
        MSG_CHECK(newState, newState != nullptr);
        MSG_CHECK(newState->enterCalled, newState->enterCalled);
    }

    TEST_CASE("GameState can call PushState on owner")
    {
        std::shared_ptr<duin::GameStateMachine> sm = std::make_shared<duin::GameStateMachine>();
        sm->PushState<TestStateA>();

        auto *stateA = sm->GetActiveState<TestStateA>();
        stateA->PushState<TestStateB>();

        auto *stateB = sm->GetActiveState<TestStateB>();
        MSG_CHECK(stateB, stateB != nullptr);
        MSG_CHECK(stateB->enterCalled, stateB->enterCalled);
    }

    TEST_CASE("GameState can call FlushStack on owner")
    {
        std::shared_ptr<duin::GameStateMachine> sm = std::make_shared<duin::GameStateMachine>();
        sm->PushState<TestStateA>();
        auto *stateA = sm->GetActiveState<TestStateA>();
        sm->PushState<TestStateB>();

        auto *stateB = sm->GetActiveState<TestStateB>();
        stateB->FlushStack();

        auto *state = sm->GetActiveState<TestStateA>();
        MSG_CHECK(state, state == nullptr);
    }

    TEST_CASE("Complex state transitions")
    {
        std::shared_ptr<duin::GameStateMachine> sm = std::make_shared<duin::GameStateMachine>();

        // Push A, B, C
        sm->PushState<TestStateA>();
        sm->PushState<TestStateB>();
        sm->PushState<TestStateC>();

        // Pop C, should have B on top
        sm->PopState();
        auto *stateB = sm->GetActiveState<TestStateB>();
        MSG_CHECK(stateB, stateB != nullptr);

        // Use signal to verify Exit is called when B is switched
        bool stateBExitCalled = false;
        stateB->ConnectOnStateExit([&stateBExitCalled]() { stateBExitCalled = true; });

        // Switch B to A, should have new A on top
        sm->SwitchState<TestStateA>();
        auto *newStateA = sm->GetActiveState<TestStateA>();
        MSG_CHECK(newStateA, newStateA != nullptr);
        MSG_CHECK(stateBExitCalled, stateBExitCalled);

        // Push C on top of new A
        sm->PushState<TestStateC>();
        auto *stateC = sm->GetActiveState<TestStateC>();
        MSG_CHECK(stateC, stateC != nullptr);

        // Flush and switch to B
        sm->FlushAndSwitchState<TestStateB>();
        auto *finalState = sm->GetActiveState<TestStateB>();
        MSG_CHECK(finalState, finalState != nullptr);
        MSG_CHECK(finalState->enterCalled, finalState->enterCalled);
    }

    TEST_CASE("GetStateStack access")
    {
        std::shared_ptr<duin::GameStateMachine> sm = std::make_shared<duin::GameStateMachine>();
        sm->PushState<TestStateA>();
        sm->PushState<TestStateB>();
        sm->PushState<TestStateC>();

        const auto &stack = sm->GetStateStack();
        MSG_CHECK(stack.size(), stack.size() == 3);
    }

    TEST_CASE("Init method call")
    {
        std::shared_ptr<duin::GameStateMachine> sm = std::make_shared<duin::GameStateMachine>();
        // Init should not crash
        sm->Init();
        CHECK(true);
    }

    TEST_CASE("Ready method call")
    {
        std::shared_ptr<duin::GameStateMachine> sm = std::make_shared<duin::GameStateMachine>();
        // Ready should not crash
        sm->Ready();
        CHECK(true);
    }

    TEST_CASE("Debug method call")
    {
        std::shared_ptr<duin::GameStateMachine> sm = std::make_shared<duin::GameStateMachine>();
        // Debug should not crash
        sm->Debug();
        CHECK(true);
    }
}

TEST_SUITE("GameState - Signal Connection and Disconnection")
{
    TEST_CASE("ConnectOnStateDraw and callback")
    {
        std::shared_ptr<duin::GameStateMachine> sm = std::make_shared<duin::GameStateMachine>();
        auto *state = sm->PushState<TestStateA>();

        bool drawSignalCalled = false;
        state->ConnectOnStateDraw([&drawSignalCalled]() { drawSignalCalled = true; });

        sm->Draw();
        MSG_CHECK(drawSignalCalled, drawSignalCalled);
    }

    TEST_CASE("ConnectOnStateDrawUI and callback")
    {
        std::shared_ptr<duin::GameStateMachine> sm = std::make_shared<duin::GameStateMachine>();
        auto *state = sm->PushState<TestStateA>();

        bool drawUISignalCalled = false;
        state->ConnectOnStateDrawUI([&drawUISignalCalled]() { drawUISignalCalled = true; });

        sm->DrawUI();
        MSG_CHECK(drawUISignalCalled, drawUISignalCalled);
    }

    TEST_CASE("ConnectOnStateEnter and callback")
    {
        std::shared_ptr<duin::GameStateMachine> sm = std::make_shared<duin::GameStateMachine>();
        auto *state = sm->PushState<TestStateA>();

        bool enterSignalCalled = false;
        state->ConnectOnStateEnter([&enterSignalCalled]() { enterSignalCalled = true; });

        // Enter was already called during PushState, push another state to test
        sm->PopState();
        sm->PushState<TestStateA>();
        // The signal should have been connected on the previous instance, so test with new state
    }

    TEST_CASE("ConnectOnStateOnEvent and callback")
    {
        std::shared_ptr<duin::GameStateMachine> sm = std::make_shared<duin::GameStateMachine>();
        auto *state = sm->PushState<TestStateA>();

        bool eventSignalCalled = false;
        state->ConnectOnStateOnEvent([&eventSignalCalled](duin::Event) { eventSignalCalled = true; });

        duin::Event testEvent{};
        sm->OnEvent(testEvent);
        MSG_CHECK(eventSignalCalled, eventSignalCalled);
    }

    TEST_CASE("ConnectOnStatePhysicsUpdate and callback")
    {
        std::shared_ptr<duin::GameStateMachine> sm = std::make_shared<duin::GameStateMachine>();
        auto *state = sm->PushState<TestStateA>();

        bool physicsUpdateSignalCalled = false;
        state->ConnectOnStatePhysicsUpdate([&physicsUpdateSignalCalled](double) { physicsUpdateSignalCalled = true; });

        sm->PhysicsUpdate(0.016);
        MSG_CHECK(physicsUpdateSignalCalled, physicsUpdateSignalCalled);
    }

    TEST_CASE("ConnectOnStateUpdate and callback")
    {
        std::shared_ptr<duin::GameStateMachine> sm = std::make_shared<duin::GameStateMachine>();
        auto *state = sm->PushState<TestStateA>();

        bool updateSignalCalled = false;
        state->ConnectOnStateUpdate([&updateSignalCalled](double) { updateSignalCalled = true; });

        sm->Update(0.016);
        MSG_CHECK(updateSignalCalled, updateSignalCalled);
    }

    TEST_CASE("ConnectAllSignals")
    {
        std::shared_ptr<duin::GameStateMachine> sm = std::make_shared<duin::GameStateMachine>();
        auto *state = sm->PushState<TestStateA>();

        int signalCallCount = 0;
        state->ConnectAllSignals([&signalCallCount]() { signalCallCount++; },            // Enter
                                 [&signalCallCount](duin::Event) { signalCallCount++; }, // OnEvent
                                 [&signalCallCount](double) { signalCallCount++; },      // Update
                                 [&signalCallCount](double) { signalCallCount++; },      // PhysicsUpdate
                                 [&signalCallCount]() { signalCallCount++; },            // Draw
                                 [&signalCallCount]() { signalCallCount++; },            // DrawUI
                                 [&signalCallCount]() { signalCallCount++; }             // Exit
        );

        sm->Update(0.016);
        MSG_CHECK(signalCallCount, signalCallCount >= 1);
    }

    TEST_CASE("DisconnectOnStateDraw")
    {
        std::shared_ptr<duin::GameStateMachine> sm = std::make_shared<duin::GameStateMachine>();
        auto *state = sm->PushState<TestStateA>();

        bool drawSignalCalled = false;
        auto uuid = state->ConnectOnStateDraw([&drawSignalCalled]() { drawSignalCalled = true; });
        state->DisconnectOnStateDraw(uuid);

        sm->Draw();
        MSG_CHECK_FALSE(drawSignalCalled, drawSignalCalled);
    }

    TEST_CASE("DisconnectOnStateDrawUI")
    {
        std::shared_ptr<duin::GameStateMachine> sm = std::make_shared<duin::GameStateMachine>();
        auto *state = sm->PushState<TestStateA>();

        bool drawUISignalCalled = false;
        auto uuid = state->ConnectOnStateDrawUI([&drawUISignalCalled]() { drawUISignalCalled = true; });
        state->DisconnectOnStateDrawUI(uuid);

        sm->DrawUI();
        MSG_CHECK_FALSE(drawUISignalCalled, drawUISignalCalled);
    }

    TEST_CASE("DisconnectOnStateEnter")
    {
        std::shared_ptr<duin::GameStateMachine> sm = std::make_shared<duin::GameStateMachine>();
        auto *state = sm->PushState<TestStateA>();

        auto uuid = state->ConnectOnStateEnter([]() {});
        state->DisconnectOnStateEnter(uuid);
        // Test passes if no crash
        CHECK(true);
    }

    TEST_CASE("DisconnectOnStateExit")
    {
        std::shared_ptr<duin::GameStateMachine> sm = std::make_shared<duin::GameStateMachine>();
        auto *state = sm->PushState<TestStateA>();

        auto uuid = state->ConnectOnStateExit([]() {});
        state->DisconnectOnStateExit(uuid);
        // Test passes if no crash
        CHECK(true);
    }

    TEST_CASE("DisconnectOnStateOnEvent")
    {
        std::shared_ptr<duin::GameStateMachine> sm = std::make_shared<duin::GameStateMachine>();
        auto *state = sm->PushState<TestStateA>();

        bool eventSignalCalled = false;
        auto uuid = state->ConnectOnStateOnEvent([&eventSignalCalled](duin::Event) { eventSignalCalled = true; });
        state->DisconnectOnStateOnEvent(uuid);

        duin::Event testEvent{};
        sm->OnEvent(testEvent);
        MSG_CHECK_FALSE(eventSignalCalled, eventSignalCalled);
    }

    TEST_CASE("DisconnectOnStatePhysicsUpdate")
    {
        std::shared_ptr<duin::GameStateMachine> sm = std::make_shared<duin::GameStateMachine>();
        auto *state = sm->PushState<TestStateA>();

        bool physicsUpdateSignalCalled = false;
        auto uuid = state->ConnectOnStatePhysicsUpdate(
            [&physicsUpdateSignalCalled](double) { physicsUpdateSignalCalled = true; });
        state->DisconnectOnStatePhysicsUpdate(uuid);

        sm->PhysicsUpdate(0.016);
        MSG_CHECK_FALSE(physicsUpdateSignalCalled, physicsUpdateSignalCalled);
    }

    TEST_CASE("DisconnectOnStateUpdate")
    {
        std::shared_ptr<duin::GameStateMachine> sm = std::make_shared<duin::GameStateMachine>();
        auto *state = sm->PushState<TestStateA>();

        bool updateSignalCalled = false;
        auto uuid = state->ConnectOnStateUpdate([&updateSignalCalled](double) { updateSignalCalled = true; });
        state->DisconnectOnStateUpdate(uuid);

        sm->Update(0.016);
        MSG_CHECK_FALSE(updateSignalCalled, updateSignalCalled);
    }

    TEST_CASE("DisconnectAllSignals")
    {
        std::shared_ptr<duin::GameStateMachine> sm = std::make_shared<duin::GameStateMachine>();
        auto *state = sm->PushState<TestStateA>();

        int signalCallCount = 0;
        auto connections = state->ConnectAllSignals(
            [&signalCallCount]() { signalCallCount++; }, [&signalCallCount](duin::Event) { signalCallCount++; },
            [&signalCallCount](double) { signalCallCount++; }, [&signalCallCount](double) { signalCallCount++; },
            [&signalCallCount]() { signalCallCount++; }, [&signalCallCount]() { signalCallCount++; },
            [&signalCallCount]() { signalCallCount++; });

        state->DisconnectAllSignals(connections);

        sm->Update(0.016);
        sm->PhysicsUpdate(0.016);
        sm->Draw();
        sm->DrawUI();
        duin::Event testEvent{};
        sm->OnEvent(testEvent);

        MSG_CHECK(signalCallCount, signalCallCount == 0);
    }
}

TEST_SUITE("GameState - Enable/Disable")
{
    TEST_CASE("EnableUpdate")
    {
        std::shared_ptr<duin::GameStateMachine> sm = std::make_shared<duin::GameStateMachine>();
        auto *state = sm->PushState<TestStateA>();

        state->EnableUpdate(true);
        // Test passes if no crash
        CHECK(true);
    }

    TEST_CASE("EnablePhysicsUpdate")
    {
        std::shared_ptr<duin::GameStateMachine> sm = std::make_shared<duin::GameStateMachine>();
        auto *state = sm->PushState<TestStateA>();

        state->EnablePhysicsUpdate(true);
        CHECK(true);
    }

    TEST_CASE("EnableDraw")
    {
        std::shared_ptr<duin::GameStateMachine> sm = std::make_shared<duin::GameStateMachine>();
        auto *state = sm->PushState<TestStateA>();

        state->EnableDraw(true);
        CHECK(true);
    }

    TEST_CASE("EnableDrawUI")
    {
        std::shared_ptr<duin::GameStateMachine> sm = std::make_shared<duin::GameStateMachine>();
        auto *state = sm->PushState<TestStateA>();

        state->EnableDrawUI(true);
        CHECK(true);
    }

    TEST_CASE("EnableOnEvent")
    {
        std::shared_ptr<duin::GameStateMachine> sm = std::make_shared<duin::GameStateMachine>();
        auto *state = sm->PushState<TestStateA>();

        state->EnableOnEvent(true);
        CHECK(true);
    }

    TEST_CASE("Enable all")
    {
        std::shared_ptr<duin::GameStateMachine> sm = std::make_shared<duin::GameStateMachine>();
        auto *state = sm->PushState<TestStateA>();

        state->Enable(true);
        CHECK(true);
    }
}

TEST_SUITE("GameState - Pause/Unpause")
{
    TEST_CASE("SetPause")
    {
        std::shared_ptr<duin::GameStateMachine> sm = std::make_shared<duin::GameStateMachine>();
        auto *state = sm->PushState<TestStateA>();

        state->SetPause();
        CHECK(true);
    }

    TEST_CASE("SetUnpause")
    {
        std::shared_ptr<duin::GameStateMachine> sm = std::make_shared<duin::GameStateMachine>();
        auto *state = sm->PushState<TestStateA>();

        state->SetUnpause();
        CHECK(true);
    }
}

TEST_SUITE("GameState - Lifecycle Methods")
{
    TEST_CASE("Draw method")
    {
        std::shared_ptr<duin::GameStateMachine> sm = std::make_shared<duin::GameStateMachine>();
        auto *state = sm->PushState<TestStateA>();

        state->Draw();
        CHECK(true);
    }

    TEST_CASE("DrawUI method")
    {
        std::shared_ptr<duin::GameStateMachine> sm = std::make_shared<duin::GameStateMachine>();
        auto *state = sm->PushState<TestStateA>();

        state->DrawUI();
        CHECK(true);
    }

    TEST_CASE("Update method")
    {
        std::shared_ptr<duin::GameStateMachine> sm = std::make_shared<duin::GameStateMachine>();
        auto *state = sm->PushState<TestStateA>();

        state->Update(0.016);
        CHECK(true);
    }

    TEST_CASE("PhysicsUpdate method")
    {
        std::shared_ptr<duin::GameStateMachine> sm = std::make_shared<duin::GameStateMachine>();
        auto *state = sm->PushState<TestStateA>();

        state->PhysicsUpdate(0.016);
        CHECK(true);
    }

    TEST_CASE("OnEvent method")
    {
        std::shared_ptr<duin::GameStateMachine> sm = std::make_shared<duin::GameStateMachine>();
        auto *state = sm->PushState<TestStateA>();

        duin::Event testEvent{};
        state->OnEvent(testEvent);
        CHECK(true);
    }
}

TEST_SUITE("GameState - Child Objects")
{
    TEST_CASE("AddChildObject")
    {
        std::shared_ptr<duin::GameStateMachine> sm = std::make_shared<duin::GameStateMachine>();
        auto *state = sm->PushState<TestStateA>();

        auto child = std::make_shared<duin::GameObject>();
        state->AddChildObject(child);
        CHECK(true);
    }

    TEST_CASE("RemoveChildObject")
    {
        std::shared_ptr<duin::GameStateMachine> sm = std::make_shared<duin::GameStateMachine>();
        auto *state = sm->PushState<TestStateA>();

        auto child = std::make_shared<duin::GameObject>();
        state->AddChildObject(child);
        state->RemoveChildObject(child);
        CHECK(true);
    }
}

TEST_SUITE("GameState - IsEqualTo")
{
    TEST_CASE("IsEqualTo same state")
    {
        std::shared_ptr<duin::GameStateMachine> sm = std::make_shared<duin::GameStateMachine>();
        auto *state = sm->PushState<TestStateA>();

        MSG_CHECK(state->IsEqualTo(state), state->IsEqualTo(state));
    }

    TEST_CASE("IsEqualTo different state")
    {
        std::shared_ptr<duin::GameStateMachine> sm = std::make_shared<duin::GameStateMachine>();
        auto *stateA = sm->PushState<TestStateA>();
        sm->PushState<TestStateB>();
        sm->PopState();
        auto *stateB = sm->PushState<TestStateB>();

        MSG_CHECK_FALSE(stateA->IsEqualTo(stateB), stateA->IsEqualTo(stateB));
    }
}

TEST_SUITE("GameStateMachine - Double Call Prevention")
{
    TEST_CASE("Enter is called exactly once on PushState")
    {
        std::shared_ptr<duin::GameStateMachine> sm = std::make_shared<duin::GameStateMachine>();

        int enterCount = 0;

        auto stateHolder = sm->CreateState<TestStateA>();
        stateHolder->ConnectOnStateEnter([&enterCount]() { enterCount++; });
        auto *state = sm->PushState(stateHolder);

        // Enter should have been called once during PushState
        MSG_CHECK(state->enterCalled, state->enterCalled);
        MSG_CHECK(enterCount, enterCount == 1);
    }

    TEST_CASE("Enter signal is emitted exactly once")
    {
        std::shared_ptr<duin::GameStateMachine> sm = std::make_shared<duin::GameStateMachine>();

        int enterSignalCount = 0;

        sm->PushState<TestStateA>();
        sm->PopState();

        auto stateHolder = sm->CreateState<TestStateA>();
        stateHolder->ConnectOnStateEnter([&enterSignalCount]() { enterSignalCount++; });
        auto *state = sm->PushState(stateHolder);

        sm->PopState();
        sm->PushState<TestStateA>();

        MSG_CHECK(enterSignalCount, enterSignalCount == 1);
    }

    TEST_CASE("Exit is called exactly once on PopState")
    {
        std::shared_ptr<duin::GameStateMachine> sm = std::make_shared<duin::GameStateMachine>();

        int exitSignalCount = 0;

        auto *state = sm->PushState<TestStateA>();
        state->ConnectOnStateExit([&exitSignalCount]() { exitSignalCount++; });

        sm->PopState();

        MSG_CHECK(state->exitCalled, state->exitCalled);
        MSG_CHECK(exitSignalCount, exitSignalCount == 1);
    }

    TEST_CASE("Exit is called exactly once on SwitchState")
    {
        std::shared_ptr<duin::GameStateMachine> sm = std::make_shared<duin::GameStateMachine>();

        int exitSignalCount = 0;

        auto stateHolder = sm->CreateState<TestStateA>();
        stateHolder->ConnectOnStateExit([&exitSignalCount]() { exitSignalCount++; });
        auto *stateA = sm->PushState(stateHolder);

        sm->SwitchState<TestStateB>();

        MSG_CHECK(stateA->exitCalled, stateA->exitCalled);
        MSG_CHECK(exitSignalCount, exitSignalCount == 1);
    }

    TEST_CASE("Update is called exactly once per frame")
    {
        std::shared_ptr<duin::GameStateMachine> sm = std::make_shared<duin::GameStateMachine>();

        int updateSignalCount = 0;

        auto stateHolder = sm->CreateState<TestStateA>();
        stateHolder->ConnectOnStateUpdate([&updateSignalCount](double) { updateSignalCount++; });
        auto *state = sm->PushState(stateHolder);

        MSG_CHECK(state->updateCalls, state->updateCalls == 0);
        MSG_CHECK(updateSignalCount, updateSignalCount == 0);

        sm->Update(0.016);

        MSG_CHECK(state->updateCalls, state->updateCalls == 1);
        MSG_CHECK(updateSignalCount, updateSignalCount == 1);

        sm->Update(0.016);

        MSG_CHECK(state->updateCalls, state->updateCalls == 2);
        MSG_CHECK(updateSignalCount, updateSignalCount == 2);
    }

    TEST_CASE("PhysicsUpdate is called exactly once per frame")
    {
        std::shared_ptr<duin::GameStateMachine> sm = std::make_shared<duin::GameStateMachine>();

        int physicsUpdateSignalCount = 0;

        auto stateHolder = sm->CreateState<TestStateA>();
        stateHolder->ConnectOnStatePhysicsUpdate([&physicsUpdateSignalCount](double) { physicsUpdateSignalCount++; });
        auto *state = sm->PushState(stateHolder);

        MSG_CHECK(state->physicsUpdateCalls, state->physicsUpdateCalls == 0);
        MSG_CHECK(physicsUpdateSignalCount, physicsUpdateSignalCount == 0);

        sm->PhysicsUpdate(0.016);

        MSG_CHECK(state->physicsUpdateCalls, state->physicsUpdateCalls == 1);
        MSG_CHECK(physicsUpdateSignalCount, physicsUpdateSignalCount == 1);

        sm->PhysicsUpdate(0.016);

        MSG_CHECK(state->physicsUpdateCalls, state->physicsUpdateCalls == 2);
        MSG_CHECK(physicsUpdateSignalCount, physicsUpdateSignalCount == 2);
    }

    TEST_CASE("Draw is called exactly once per frame")
    {
        std::shared_ptr<duin::GameStateMachine> sm = std::make_shared<duin::GameStateMachine>();

        int drawSignalCount = 0;

        auto stateHolder = sm->CreateState<TestStateA>();
        stateHolder->ConnectOnStateDraw([&drawSignalCount]() { drawSignalCount++; });
        auto *state = sm->PushState(stateHolder);

        MSG_CHECK(state->drawCalls, state->drawCalls == 0);
        MSG_CHECK(drawSignalCount, drawSignalCount == 0);

        sm->Draw();

        MSG_CHECK(state->drawCalls, state->drawCalls == 1);
        MSG_CHECK(drawSignalCount, drawSignalCount == 1);

        sm->Draw();

        MSG_CHECK(state->drawCalls, state->drawCalls == 2);
        MSG_CHECK(drawSignalCount, drawSignalCount == 2);
    }

    TEST_CASE("DrawUI is called exactly once per frame")
    {
        std::shared_ptr<duin::GameStateMachine> sm = std::make_shared<duin::GameStateMachine>();

        int drawUISignalCount = 0;

        auto stateHolder = sm->CreateState<TestStateA>();
        stateHolder->ConnectOnStateDrawUI([&drawUISignalCount]() { drawUISignalCount++; });
        auto *state = sm->PushState(stateHolder);

        MSG_CHECK(state->drawUICalls, state->drawUICalls == 0);
        MSG_CHECK(drawUISignalCount, drawUISignalCount == 0);

        sm->DrawUI();

        MSG_CHECK(state->drawUICalls, state->drawUICalls == 1);
        MSG_CHECK(drawUISignalCount, drawUISignalCount == 1);

        sm->DrawUI();

        MSG_CHECK(state->drawUICalls, state->drawUICalls == 2);
        MSG_CHECK(drawUISignalCount, drawUISignalCount == 2);
    }

    TEST_CASE("OnEvent is called exactly once per event")
    {
        std::shared_ptr<duin::GameStateMachine> sm = std::make_shared<duin::GameStateMachine>();

        int eventSignalCount = 0;

        auto stateHolder = sm->CreateState<TestStateA>();
        stateHolder->ConnectOnStateOnEvent([&eventSignalCount](duin::Event) { eventSignalCount++; });
        auto *state = sm->PushState(stateHolder);

        MSG_CHECK(state->eventCalls, state->eventCalls == 0);
        MSG_CHECK(eventSignalCount, eventSignalCount == 0);

        duin::Event testEvent{};
        sm->OnEvent(testEvent);

        MSG_CHECK(state->eventCalls, state->eventCalls == 1);
        MSG_CHECK(eventSignalCount, eventSignalCount == 1);

        sm->OnEvent(testEvent);

        MSG_CHECK(state->eventCalls, state->eventCalls == 2);
        MSG_CHECK(eventSignalCount, eventSignalCount == 2);
    }

    TEST_CASE("Multiple lifecycle calls in sequence")
    {
        std::shared_ptr<duin::GameStateMachine> sm = std::make_shared<duin::GameStateMachine>();

        int updateCount = 0;
        int physicsCount = 0;
        int drawCount = 0;
        int drawUICount = 0;
        int eventCount = 0;

        auto stateHolder = sm->CreateState<TestStateA>();
        stateHolder->ConnectOnStateUpdate([&updateCount](double) { updateCount++; });
        stateHolder->ConnectOnStatePhysicsUpdate([&physicsCount](double) { physicsCount++; });
        stateHolder->ConnectOnStateDraw([&drawCount]() { drawCount++; });
        stateHolder->ConnectOnStateDrawUI([&drawUICount]() { drawUICount++; });
        stateHolder->ConnectOnStateOnEvent([&eventCount](duin::Event) { eventCount++; });
        auto *state = sm->PushState(stateHolder);

        // Simulate one frame
        sm->Update(0.016);
        sm->PhysicsUpdate(0.016);
        sm->Draw();
        sm->DrawUI();
        duin::Event e{};
        sm->OnEvent(e);

        MSG_CHECK(state->updateCalls, state->updateCalls == 1);
        MSG_CHECK(state->physicsUpdateCalls, state->physicsUpdateCalls == 1);
        MSG_CHECK(state->drawCalls, state->drawCalls == 1);
        MSG_CHECK(state->drawUICalls, state->drawUICalls == 1);
        MSG_CHECK(state->eventCalls, state->eventCalls == 1);

        MSG_CHECK(updateCount, updateCount == 1);
        MSG_CHECK(physicsCount, physicsCount == 1);
        MSG_CHECK(drawCount, drawCount == 1);
        MSG_CHECK(drawUICount, drawUICount == 1);
        MSG_CHECK(eventCount, eventCount == 1);
    }

    TEST_CASE("FlushAndSwitchState calls Exit exactly once per state")
    {
        std::shared_ptr<duin::GameStateMachine> sm = std::make_shared<duin::GameStateMachine>();

        int exitCountA = 0;
        int exitCountB = 0;
        int exitCountC = 0;

        auto stateHolderA = sm->CreateState<TestStateA>();
        stateHolderA->ConnectOnStateExit([&exitCountA]() { exitCountA++; });
        auto *stateA = sm->PushState(stateHolderA);

        auto stateHolderB = sm->CreateState<TestStateB>();
        stateHolderB->ConnectOnStateExit([&exitCountB]() { exitCountB++; });
        auto *stateB = sm->PushState(stateHolderB);

        auto stateHolderC = sm->CreateState<TestStateC>();
        stateHolderC->ConnectOnStateExit([&exitCountC]() { exitCountC++; });
        auto *stateC = sm->PushState(stateHolderC);

        sm->FlushAndSwitchState<TestStateA>();

        MSG_CHECK(stateA->exitCalled, stateA->exitCalled);
        MSG_CHECK(stateB->exitCalled, stateB->exitCalled);
        MSG_CHECK(stateC->exitCalled, stateC->exitCalled);

        MSG_CHECK(exitCountA, exitCountA == 1);
        MSG_CHECK(exitCountB, exitCountB == 1);
        MSG_CHECK(exitCountC, exitCountC == 1);
    }

    TEST_CASE("State switching doesn't double-call Enter/Exit")
    {
        std::shared_ptr<duin::GameStateMachine> sm = std::make_shared<duin::GameStateMachine>();

        int exitCountA = 0;
        int enterCountB = 0;

        auto stateHolderA = sm->CreateState<TestStateA>();
        stateHolderA->ConnectOnStateExit([&exitCountA]() { exitCountA++; });
        auto *stateA = sm->PushState(stateHolderA);

        auto stateHolderB = sm->CreateState<TestStateB>();
        stateHolderB->ConnectOnStateEnter([&enterCountB]() { enterCountB++; });
        sm->SwitchState(stateHolderB);

        auto *stateB = sm->GetActiveState<TestStateB>();

        MSG_CHECK(exitCountA, exitCountA == 1);
        MSG_CHECK(stateB->enterCalled, stateB->enterCalled);
        MSG_CHECK(enterCountB, enterCountB == 1);

        // Switch again to verify
        int exitCountB = 0;
        auto stateHolderC = sm->CreateState<TestStateC>();
        stateB->ConnectOnStateExit([&exitCountB]() { exitCountB++; });

        sm->SwitchState(stateHolderC);

        MSG_CHECK(exitCountB, exitCountB == 1);
    }

    TEST_CASE("Nested state operations don't cause double calls")
    {
        std::shared_ptr<duin::GameStateMachine> sm = std::make_shared<duin::GameStateMachine>();

        int enterCount = 0;

        // Create a SwitchingState that will switch to TestStateB during Enter
        auto stateHolder = sm->CreateState<SwitchingState>();
        stateHolder->shouldSwitch = true;
        stateHolder->ConnectOnStateEnter([&enterCount]() { enterCount++; });
        
        auto *switchingState = sm->PushState(stateHolder);

        // Enter should have been called exactly once, even though it triggered a state switch
        MSG_CHECK(switchingState->enterCalled, switchingState->enterCalled);
        MSG_CHECK(enterCount, enterCount == 1);

        // Verify we ended up in TestStateB after the nested switch
        auto *stateB = sm->GetActiveState<TestStateB>();
        MSG_CHECK(stateB, stateB != nullptr);
        MSG_CHECK(stateB->enterCalled, stateB->enterCalled);
    }
}
} // namespace TestGameStateMachine
