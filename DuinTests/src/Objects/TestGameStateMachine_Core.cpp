#include "TestGameStateMachineDefinitions.h"

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

} // namespace TestGameStateMachine
