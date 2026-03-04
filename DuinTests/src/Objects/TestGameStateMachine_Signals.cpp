#include "TestGameStateMachineDefinitions.h"

namespace TestGameStateMachine
{

TEST_SUITE("GameState - Signal Connection and Disconnection")
{
    TEST_CASE("ConnectOnStateEnter and callback")
    {
        std::shared_ptr<duin::GameStateMachine> sm = std::make_shared<duin::GameStateMachine>();

        bool enterSignalCalled = false;
        auto stateHolder = sm->CreateState<TestStateA>();
        stateHolder->ConnectOnStateEnter([&enterSignalCalled]() { enterSignalCalled = true; });
        sm->PushState(stateHolder);

        MSG_CHECK(enterSignalCalled, enterSignalCalled);
    }

    TEST_CASE("ConnectOnStateExit and callback")
    {
        std::shared_ptr<duin::GameStateMachine> sm = std::make_shared<duin::GameStateMachine>();
        auto *state = sm->PushState<TestStateA>();

        bool exitSignalCalled = false;
        state->ConnectOnStateExit([&exitSignalCalled]() { exitSignalCalled = true; });

        sm->PopState();
        MSG_CHECK(exitSignalCalled, exitSignalCalled);
    }

    TEST_CASE("DisconnectOnStateEnter")
    {
        std::shared_ptr<duin::GameStateMachine> sm = std::make_shared<duin::GameStateMachine>();

        bool enterSignalCalled = false;
        auto stateHolder = sm->CreateState<TestStateA>();
        auto uuid = stateHolder->ConnectOnStateEnter([&enterSignalCalled]() { enterSignalCalled = true; });
        stateHolder->DisconnectOnStateEnter(uuid);
        sm->PushState(stateHolder);

        MSG_CHECK_FALSE(enterSignalCalled, enterSignalCalled);
    }

    TEST_CASE("DisconnectOnStateExit")
    {
        std::shared_ptr<duin::GameStateMachine> sm = std::make_shared<duin::GameStateMachine>();
        auto *state = sm->PushState<TestStateA>();

        bool exitSignalCalled = false;
        auto uuid = state->ConnectOnStateExit([&exitSignalCalled]() { exitSignalCalled = true; });
        state->DisconnectOnStateExit(uuid);

        sm->PopState();
        MSG_CHECK_FALSE(exitSignalCalled, exitSignalCalled);
    }

    TEST_CASE("ConnectAllSignals")
    {
        std::shared_ptr<duin::GameStateMachine> sm = std::make_shared<duin::GameStateMachine>();

        int enterCount = 0;
        int exitCount  = 0;

        auto stateHolder = sm->CreateState<TestStateA>();
        stateHolder->ConnectAllSignals([&enterCount]() { enterCount++; },
                                       [&exitCount]()  { exitCount++;  });
        sm->PushState(stateHolder);
        sm->PopState();

        MSG_CHECK(enterCount, enterCount == 1);
        MSG_CHECK(exitCount,  exitCount  == 1);
    }

    TEST_CASE("DisconnectAllSignals")
    {
        std::shared_ptr<duin::GameStateMachine> sm = std::make_shared<duin::GameStateMachine>();

        int enterCount = 0;
        int exitCount  = 0;

        auto stateHolder = sm->CreateState<TestStateA>();
        auto connections = stateHolder->ConnectAllSignals([&enterCount]() { enterCount++; },
                                                          [&exitCount]()  { exitCount++;  });
        stateHolder->DisconnectAllSignals(connections);

        sm->PushState(stateHolder);
        sm->PopState();

        MSG_CHECK(enterCount, enterCount == 0);
        MSG_CHECK(exitCount,  exitCount  == 0);
    }
}

} // namespace TestGameStateMachine
