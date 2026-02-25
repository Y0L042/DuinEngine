#include "TestGameStateMachineDefinitions.h"

namespace TestGameStateMachine
{

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

} // namespace TestGameStateMachine
