#include "TestGameStateMachineDefinitions.h"

namespace TestGameStateMachine
{

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

TEST_SUITE("GameStateMachine - Child Count as States are Pushed and Popped")
{
    TEST_CASE("PushState adds the state as a child of the sm")
    {
        auto sm = std::make_shared<duin::GameStateMachine>();
        std::weak_ptr<duin::GameState> weak;

        {
            // Capture weak before local handle is dropped
            auto holder = sm->CreateState<TestStateA>();
            weak = holder;
            sm->PushState(holder);
        }
        // sm now holds the sole reference
        MSG_CHECK(weak.expired(), !weak.expired());
        MSG_CHECK(sm->GetStateStack().size(), sm->GetStateStack().size() == 1);
    }

    TEST_CASE("Each PushState adds one more child")
    {
        auto sm = std::make_shared<duin::GameStateMachine>();

        sm->PushState<TestStateA>();
        MSG_CHECK(sm->GetStateStack().size(), sm->GetStateStack().size() == 1);
        MSG_CHECK(sm->GetChildrenCount(), sm->GetChildrenCount() == 1);
        MSG_CHECK(sm->GetChildren().size(), sm->GetChildren().size() == 1);

        sm->PushState<TestStateB>();
        MSG_CHECK(sm->GetStateStack().size(), sm->GetStateStack().size() == 2);
        MSG_CHECK(sm->GetChildrenCount(), sm->GetChildrenCount() == 2);
        MSG_CHECK(sm->GetChildren().size(), sm->GetChildren().size() == 2);

        sm->PushState<TestStateC>();
        MSG_CHECK(sm->GetStateStack().size(), sm->GetStateStack().size() == 3);
        MSG_CHECK(sm->GetChildrenCount(), sm->GetChildrenCount() == 3);
        MSG_CHECK(sm->GetChildren().size(), sm->GetChildren().size() == 3);
    }

    TEST_CASE("PopState removes the state as a child of the sm")
    {
        auto sm = std::make_shared<duin::GameStateMachine>();

        std::weak_ptr<duin::GameState> weakA;
        {
            auto holder = sm->CreateState<TestStateA>();
            weakA = holder;
            sm->PushState(holder);
        }

        MSG_CHECK(weakA.expired(), !weakA.expired());
        MSG_CHECK(sm->GetChildrenCount(), sm->GetChildrenCount() == 1);
        MSG_CHECK(sm->GetChildren().size(), sm->GetChildren().size() == 1);

        sm->PopState();

        // sm no longer holds a reference — state must be gone
        MSG_CHECK(weakA.expired(), weakA.expired());
        MSG_CHECK(sm->GetStateStack().size(), sm->GetStateStack().size() == 0);
        MSG_CHECK(sm->GetChildrenCount(), sm->GetChildrenCount() == 0);
        MSG_CHECK(sm->GetChildren().size(), sm->GetChildren().size() == 0);
    }

    TEST_CASE("PopState on a two-deep stack removes only the top state")
    {
        auto sm = std::make_shared<duin::GameStateMachine>();

        std::weak_ptr<duin::GameState> weakA, weakB;
        {
            auto holderA = sm->CreateState<TestStateA>();
            weakA = holderA;
            sm->PushState(holderA);

            auto holderB = sm->CreateState<TestStateB>();
            weakB = holderB;
            sm->PushState(holderB);
        }

        MSG_CHECK(sm->GetStateStack().size(), sm->GetStateStack().size() == 2);
        MSG_CHECK(sm->GetChildrenCount(), sm->GetChildrenCount() == 2);
        MSG_CHECK(sm->GetChildren().size(), sm->GetChildren().size() == 2);
        MSG_CHECK(weakA.expired(), !weakA.expired());
        MSG_CHECK(weakB.expired(), !weakB.expired());

        sm->PopState();

        MSG_CHECK(sm->GetStateStack().size(), sm->GetStateStack().size() == 1);
        MSG_CHECK(sm->GetChildrenCount(), sm->GetChildrenCount() == 1);
        MSG_CHECK(sm->GetChildren().size(), sm->GetChildren().size() == 1);
        MSG_CHECK(weakA.expired(), !weakA.expired()); // stateA still owned by sm
        MSG_CHECK(weakB.expired(), weakB.expired());  // stateB released
    }

    TEST_CASE("SwitchState releases the old state and owns the new one")
    {
        auto sm = std::make_shared<duin::GameStateMachine>();

        std::weak_ptr<duin::GameState> weakA, weakB;
        {
            auto holderA = sm->CreateState<TestStateA>();
            weakA = holderA;
            sm->PushState(holderA);

            auto holderB = sm->CreateState<TestStateB>();
            weakB = holderB;
            sm->SwitchState(holderB);
        }

        MSG_CHECK(sm->GetStateStack().size(), sm->GetStateStack().size() == 1);
        MSG_CHECK(sm->GetChildrenCount(), sm->GetChildrenCount() == 1);
        MSG_CHECK(sm->GetChildren().size(), sm->GetChildren().size() == 1);
        MSG_CHECK(weakA.expired(), weakA.expired());  // old state released
        MSG_CHECK(weakB.expired(), !weakB.expired()); // new state owned by sm
    }

    TEST_CASE("SwitchState on a two-deep stack releases only the top, keeps the bottom")
    {
        auto sm = std::make_shared<duin::GameStateMachine>();

        std::weak_ptr<duin::GameState> weakA, weakB, weakC;
        {
            auto holderA = sm->CreateState<TestStateA>();
            weakA = holderA;
            sm->PushState(holderA);

            auto holderB = sm->CreateState<TestStateB>();
            weakB = holderB;
            sm->PushState(holderB); // A paused, B on top

            auto holderC = sm->CreateState<TestStateC>();
            weakC = holderC;
            sm->SwitchState(holderC); // B released, C on top; A still in stack
        }

        MSG_CHECK(sm->GetStateStack().size(), sm->GetStateStack().size() == 2);
        MSG_CHECK(sm->GetChildrenCount(), sm->GetChildrenCount() == 2);
        MSG_CHECK(sm->GetChildren().size(), sm->GetChildren().size() == 2);
        MSG_CHECK(weakA.expired(), !weakA.expired()); // A still in stack (paused)
        MSG_CHECK(weakB.expired(), weakB.expired());  // B released
        MSG_CHECK(weakC.expired(), !weakC.expired()); // C on top
    }

    TEST_CASE("FlushStack releases all states")
    {
        auto sm = std::make_shared<duin::GameStateMachine>();

        std::weak_ptr<duin::GameState> weakA, weakB, weakC;
        {
            auto holderA = sm->CreateState<TestStateA>();
            weakA = holderA;
            sm->PushState(holderA);

            auto holderB = sm->CreateState<TestStateB>();
            weakB = holderB;
            sm->PushState(holderB);

            auto holderC = sm->CreateState<TestStateC>();
            weakC = holderC;
            sm->PushState(holderC);
        }

        MSG_CHECK(sm->GetStateStack().size(), sm->GetStateStack().size() == 3);
        MSG_CHECK(sm->GetChildrenCount(), sm->GetChildrenCount() == 3);
        MSG_CHECK(sm->GetChildren().size(), sm->GetChildren().size() == 3);

        sm->FlushStack();

        MSG_CHECK(sm->GetStateStack().size(), sm->GetStateStack().size() == 0);
        MSG_CHECK(sm->GetChildrenCount(), sm->GetChildrenCount() == 0);
        MSG_CHECK(sm->GetChildren().size(), sm->GetChildren().size() == 0);
        MSG_CHECK(weakA.expired(), weakA.expired());
        MSG_CHECK(weakB.expired(), weakB.expired());
        MSG_CHECK(weakC.expired(), weakC.expired());
    }

    TEST_CASE("FlushAndSwitchState releases all old states and owns exactly one new state")
    {
        auto sm = std::make_shared<duin::GameStateMachine>();

        std::weak_ptr<duin::GameState> weakA, weakB, weakC, weakNew;
        {
            auto holderA = sm->CreateState<TestStateA>();
            weakA = holderA;
            sm->PushState(holderA);

            auto holderB = sm->CreateState<TestStateB>();
            weakB = holderB;
            sm->PushState(holderB);

            auto holderC = sm->CreateState<TestStateC>();
            weakC = holderC;
            sm->PushState(holderC);

            auto holderNew = sm->CreateState<TestStateA>();
            weakNew = holderNew;
            sm->FlushAndSwitchState(holderNew);
        }

        MSG_CHECK(sm->GetStateStack().size(), sm->GetStateStack().size() == 1);
        MSG_CHECK(sm->GetChildrenCount(), sm->GetChildrenCount() == 1);
        MSG_CHECK(sm->GetChildren().size(), sm->GetChildren().size() == 1);
        MSG_CHECK(weakA.expired(),   weakA.expired());
        MSG_CHECK(weakB.expired(),   weakB.expired());
        MSG_CHECK(weakC.expired(),   weakC.expired());
        MSG_CHECK(weakNew.expired(), !weakNew.expired());
    }

    TEST_CASE("Destroying the sm releases all states currently on the stack")
    {
        std::weak_ptr<duin::GameState> weakA, weakB, weakC;

        {
            auto sm = std::make_shared<duin::GameStateMachine>();

            auto holderA = sm->CreateState<TestStateA>();
            weakA = holderA;
            sm->PushState(holderA);

            auto holderB = sm->CreateState<TestStateB>();
            weakB = holderB;
            sm->PushState(holderB);

            auto holderC = sm->CreateState<TestStateC>();
            weakC = holderC;
            sm->PushState(holderC);
        } // sm destroyed here

        MSG_CHECK(weakA.expired(), weakA.expired());
        MSG_CHECK(weakB.expired(), weakB.expired());
        MSG_CHECK(weakC.expired(), weakC.expired());
    }

    TEST_CASE("CreateState adds a child immediately before any push")
    {
        auto sm = std::make_shared<duin::GameStateMachine>();

        std::weak_ptr<duin::GameState> weak;
        {
            auto holder = sm->CreateState<TestStateA>();
            weak = holder;
            // Not pushed yet — sm holds it as a child via CreateChildObject
            MSG_CHECK(weak.expired(), !weak.expired());
        }
        // Local handle dropped; sm's children vector is the only owner
        MSG_CHECK(weak.expired(), !weak.expired());
    }

    TEST_CASE("Push and pop sequence leaves stack and child count consistent")
    {
        auto sm = std::make_shared<duin::GameStateMachine>();

        // push A
        sm->PushState<TestStateA>();
        MSG_CHECK(sm->GetStateStack().size(), sm->GetStateStack().size() == 1);
        MSG_CHECK(sm->GetChildrenCount(), sm->GetChildrenCount() == 1);
        MSG_CHECK(sm->GetChildren().size(), sm->GetChildren().size() == 1);

        // push B
        sm->PushState<TestStateB>();
        MSG_CHECK(sm->GetStateStack().size(), sm->GetStateStack().size() == 2);
        MSG_CHECK(sm->GetChildrenCount(), sm->GetChildrenCount() == 2);
        MSG_CHECK(sm->GetChildren().size(), sm->GetChildren().size() == 2);

        // push C
        sm->PushState<TestStateC>();
        MSG_CHECK(sm->GetStateStack().size(), sm->GetStateStack().size() == 3);
        MSG_CHECK(sm->GetChildrenCount(), sm->GetChildrenCount() == 3);
        MSG_CHECK(sm->GetChildren().size(), sm->GetChildren().size() == 3);

        // pop C
        sm->PopState();
        MSG_CHECK(sm->GetStateStack().size(), sm->GetStateStack().size() == 2);
        MSG_CHECK(sm->GetChildrenCount(), sm->GetChildrenCount() == 2);
        MSG_CHECK(sm->GetChildren().size(), sm->GetChildren().size() == 2);

        // pop B
        sm->PopState();
        MSG_CHECK(sm->GetStateStack().size(), sm->GetStateStack().size() == 1);
        MSG_CHECK(sm->GetChildrenCount(), sm->GetChildrenCount() == 1);
        MSG_CHECK(sm->GetChildren().size(), sm->GetChildren().size() == 1);

        // pop A
        sm->PopState();
        MSG_CHECK(sm->GetStateStack().size(), sm->GetStateStack().size() == 0);
        MSG_CHECK(sm->GetChildrenCount(), sm->GetChildrenCount() == 0);
        MSG_CHECK(sm->GetChildren().size(), sm->GetChildren().size() == 0);
    }
}

} // namespace TestGameStateMachine
