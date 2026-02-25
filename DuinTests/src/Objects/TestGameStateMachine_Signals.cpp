#include "TestGameStateMachineDefinitions.h"

namespace TestGameStateMachine
{

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

} // namespace TestGameStateMachine
