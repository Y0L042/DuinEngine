#include "TestGameStateMachineDefinitions.h"

namespace TestGameStateMachine
{

TEST_SUITE("GameStateMachine - Hierarchy Update")
{
    TEST_CASE("GameState Update called exactly once when root ObjectUpdate propagates through hierarchy")
    {
        // Root GameObject -> GameStateMachine (child) -> active GameState
        // Updating the root must NOT double-call the state's Update
        auto root = std::make_shared<duin::GameObject>();
        auto sm = root->CreateChildObject<duin::GameStateMachine>();
        auto *state = sm->PushState<TestStateA>();

        MSG_CHECK(state->updateCalls, state->updateCalls == 0);

        // ObjectUpdate propagates through the hierarchy
        root->ObjectUpdate(0.016);

        MSG_CHECK(state->updateCalls, state->updateCalls == 1);
    }

    TEST_CASE("Only active GameState and its child objects receive lifecycle callbacks via root")
    {
        // Hierarchy: root -> sm -> stateA (with childA) / stateB (with childB)
        // When stateA is active: stateA and childA receive all callbacks; stateB and childB do not.
        // After switching to stateB: stateB and childB receive all callbacks; stateA and childA do not.

        auto root = std::make_shared<duin::GameObject>();
        auto sm = root->CreateChildObject<duin::GameStateMachine>();

        // Build stateA with a child tracking object
        auto stateAHolder = sm->CreateState<TestStateA>();
        auto childA = stateAHolder->CreateChildObject<TrackingObject>();

        // Build stateB with a child tracking object (TestStateA has all counters)
        auto stateBHolder = sm->CreateState<TestStateA>();
        auto childB = stateBHolder->CreateChildObject<TrackingObject>();

        // Push stateA as active
        auto *stateA = sm->PushState(stateAHolder);
        auto *stateB = stateBHolder.get();

        duin::Event testEvent{};

        // --- stateA active ---

        root->ObjectUpdate(0.016);
        root->ObjectPhysicsUpdate(0.016);
        root->ObjectDraw();
        root->ObjectDrawUI();
        root->ObjectOnEvent(testEvent);

        MSG_CHECK(stateA->updateCalls, stateA->updateCalls == 1);
        MSG_CHECK(stateA->physicsUpdateCalls, stateA->physicsUpdateCalls == 1);
        MSG_CHECK(stateA->drawCalls, stateA->drawCalls == 1);
        MSG_CHECK(stateA->drawUICalls, stateA->drawUICalls == 1);
        MSG_CHECK(stateA->eventCalls, stateA->eventCalls == 1);

        MSG_CHECK(childA->updateCalls, childA->updateCalls == 1);
        MSG_CHECK(childA->physicsUpdateCalls, childA->physicsUpdateCalls == 1);
        MSG_CHECK(childA->drawCalls, childA->drawCalls == 1);
        MSG_CHECK(childA->drawUICalls, childA->drawUICalls == 1);
        MSG_CHECK(childA->eventCalls, childA->eventCalls == 1);

        MSG_CHECK(stateB->updateCalls, stateB->updateCalls == 0);
        MSG_CHECK(stateB->physicsUpdateCalls, stateB->physicsUpdateCalls == 0);
        MSG_CHECK(stateB->drawCalls, stateB->drawCalls == 0);
        MSG_CHECK(stateB->drawUICalls, stateB->drawUICalls == 0);
        MSG_CHECK(stateB->eventCalls, stateB->eventCalls == 0);

        MSG_CHECK(childB->updateCalls, childB->updateCalls == 0);
        MSG_CHECK(childB->physicsUpdateCalls, childB->physicsUpdateCalls == 0);
        MSG_CHECK(childB->drawCalls, childB->drawCalls == 0);
        MSG_CHECK(childB->drawUICalls, childB->drawUICalls == 0);
        MSG_CHECK(childB->eventCalls, childB->eventCalls == 0);

        // --- switch to stateB ---

        sm->SwitchState(stateBHolder);

        stateA->updateCalls = 0;
        stateA->physicsUpdateCalls = 0;
        stateA->drawCalls = 0;
        stateA->drawUICalls = 0;
        stateA->eventCalls = 0;
        childA->updateCalls = 0;
        childA->physicsUpdateCalls = 0;
        childA->drawCalls = 0;
        childA->drawUICalls = 0;
        childA->eventCalls = 0;
        stateB->updateCalls = 0;
        stateB->physicsUpdateCalls = 0;
        stateB->drawCalls = 0;
        stateB->drawUICalls = 0;
        stateB->eventCalls = 0;
        childB->updateCalls = 0;
        childB->physicsUpdateCalls = 0;
        childB->drawCalls = 0;
        childB->drawUICalls = 0;
        childB->eventCalls = 0;

        root->ObjectUpdate(0.016);
        root->ObjectPhysicsUpdate(0.016);
        root->ObjectDraw();
        root->ObjectDrawUI();
        root->ObjectOnEvent(testEvent);

        MSG_CHECK(stateB->updateCalls, stateB->updateCalls == 1);
        MSG_CHECK(stateB->physicsUpdateCalls, stateB->physicsUpdateCalls == 1);
        MSG_CHECK(stateB->drawCalls, stateB->drawCalls == 1);
        MSG_CHECK(stateB->drawUICalls, stateB->drawUICalls == 1);
        MSG_CHECK(stateB->eventCalls, stateB->eventCalls == 1);

        MSG_CHECK(childB->updateCalls, childB->updateCalls == 1);
        MSG_CHECK(childB->physicsUpdateCalls, childB->physicsUpdateCalls == 1);
        MSG_CHECK(childB->drawCalls, childB->drawCalls == 1);
        MSG_CHECK(childB->drawUICalls, childB->drawUICalls == 1);
        MSG_CHECK(childB->eventCalls, childB->eventCalls == 1);

        MSG_CHECK(stateA->updateCalls, stateA->updateCalls == 0);
        MSG_CHECK(stateA->physicsUpdateCalls, stateA->physicsUpdateCalls == 0);
        MSG_CHECK(stateA->drawCalls, stateA->drawCalls == 0);
        MSG_CHECK(stateA->drawUICalls, stateA->drawUICalls == 0);
        MSG_CHECK(stateA->eventCalls, stateA->eventCalls == 0);

        MSG_CHECK(childA->updateCalls, childA->updateCalls == 0);
        MSG_CHECK(childA->physicsUpdateCalls, childA->physicsUpdateCalls == 0);
        MSG_CHECK(childA->drawCalls, childA->drawCalls == 0);
        MSG_CHECK(childA->drawUICalls, childA->drawUICalls == 0);
        MSG_CHECK(childA->eventCalls, childA->eventCalls == 0);
    }
}

TEST_SUITE("GameStateMachine - Hierarchy Edge Cases")
{
    TEST_CASE("Child added to state after it becomes active still receives callbacks")
    {
        auto root = std::make_shared<duin::GameObject>();
        auto sm = root->CreateChildObject<duin::GameStateMachine>();
        auto *state = sm->PushState<TestStateA>();

        // Child is added AFTER the state is already active
        auto lateChild = state->CreateChildObject<TrackingObject>();

        root->ObjectUpdate(0.016);
        root->ObjectPhysicsUpdate(0.016);
        root->ObjectDraw();
        root->ObjectDrawUI();
        root->ObjectOnEvent(duin::Event{});

        MSG_CHECK(lateChild->updateCalls, lateChild->updateCalls == 1);
        MSG_CHECK(lateChild->physicsUpdateCalls, lateChild->physicsUpdateCalls == 1);
        MSG_CHECK(lateChild->drawCalls, lateChild->drawCalls == 1);
        MSG_CHECK(lateChild->drawUICalls, lateChild->drawUICalls == 1);
        MSG_CHECK(lateChild->eventCalls, lateChild->eventCalls == 1);
    }

    TEST_CASE("Child removed from active state no longer receives callbacks")
    {
        auto root = std::make_shared<duin::GameObject>();
        auto sm = root->CreateChildObject<duin::GameStateMachine>();
        auto *state = sm->PushState<TestStateA>();
        auto child = state->CreateChildObject<TrackingObject>();

        // Confirm child receives callbacks while attached
        root->ObjectUpdate(0.016);
        MSG_CHECK(child->updateCalls, child->updateCalls == 1);

        // Remove child, then verify no further callbacks
        state->RemoveChildObject(child);
        child->updateCalls = 0;

        root->ObjectUpdate(0.016);
        root->ObjectPhysicsUpdate(0.016);
        root->ObjectDraw();
        root->ObjectDrawUI();
        root->ObjectOnEvent(duin::Event{});

        MSG_CHECK(child->updateCalls, child->updateCalls == 0);
        MSG_CHECK(child->physicsUpdateCalls, child->physicsUpdateCalls == 0);
        MSG_CHECK(child->drawCalls, child->drawCalls == 0);
        MSG_CHECK(child->drawUICalls, child->drawUICalls == 0);
        MSG_CHECK(child->eventCalls, child->eventCalls == 0);
    }

    TEST_CASE("Multiple children on active state each receive callbacks exactly once")
    {
        auto root = std::make_shared<duin::GameObject>();
        auto sm = root->CreateChildObject<duin::GameStateMachine>();
        auto *state = sm->PushState<TestStateA>();

        auto child1 = state->CreateChildObject<TrackingObject>();
        auto child2 = state->CreateChildObject<TrackingObject>();
        auto child3 = state->CreateChildObject<TrackingObject>();

        root->ObjectUpdate(0.016);
        root->ObjectPhysicsUpdate(0.016);
        root->ObjectDraw();
        root->ObjectDrawUI();
        root->ObjectOnEvent(duin::Event{});

        MSG_CHECK(child1->updateCalls, child1->updateCalls == 1);
        MSG_CHECK(child1->physicsUpdateCalls, child1->physicsUpdateCalls == 1);
        MSG_CHECK(child1->drawCalls, child1->drawCalls == 1);
        MSG_CHECK(child1->drawUICalls, child1->drawUICalls == 1);
        MSG_CHECK(child1->eventCalls, child1->eventCalls == 1);

        MSG_CHECK(child2->updateCalls, child2->updateCalls == 1);
        MSG_CHECK(child2->physicsUpdateCalls, child2->physicsUpdateCalls == 1);
        MSG_CHECK(child2->drawCalls, child2->drawCalls == 1);
        MSG_CHECK(child2->drawUICalls, child2->drawUICalls == 1);
        MSG_CHECK(child2->eventCalls, child2->eventCalls == 1);

        MSG_CHECK(child3->updateCalls, child3->updateCalls == 1);
        MSG_CHECK(child3->physicsUpdateCalls, child3->physicsUpdateCalls == 1);
        MSG_CHECK(child3->drawCalls, child3->drawCalls == 1);
        MSG_CHECK(child3->drawUICalls, child3->drawUICalls == 1);
        MSG_CHECK(child3->eventCalls, child3->eventCalls == 1);
    }

    TEST_CASE("Grandchildren of active state receive callbacks")
    {
        // root -> sm -> state -> child -> grandchild
        auto root = std::make_shared<duin::GameObject>();
        auto sm = root->CreateChildObject<duin::GameStateMachine>();
        auto *state = sm->PushState<TestStateA>();
        auto child = state->CreateChildObject<TrackingObject>();
        auto grandchild = child->CreateChildObject<TrackingObject>();

        root->ObjectUpdate(0.016);
        root->ObjectPhysicsUpdate(0.016);
        root->ObjectDraw();
        root->ObjectDrawUI();
        root->ObjectOnEvent(duin::Event{});

        MSG_CHECK(child->updateCalls, child->updateCalls == 1);
        MSG_CHECK(child->physicsUpdateCalls, child->physicsUpdateCalls == 1);
        MSG_CHECK(child->drawCalls, child->drawCalls == 1);
        MSG_CHECK(child->drawUICalls, child->drawUICalls == 1);
        MSG_CHECK(child->eventCalls, child->eventCalls == 1);

        MSG_CHECK(grandchild->updateCalls, grandchild->updateCalls == 1);
        MSG_CHECK(grandchild->physicsUpdateCalls, grandchild->physicsUpdateCalls == 1);
        MSG_CHECK(grandchild->drawCalls, grandchild->drawCalls == 1);
        MSG_CHECK(grandchild->drawUICalls, grandchild->drawUICalls == 1);
        MSG_CHECK(grandchild->eventCalls, grandchild->eventCalls == 1);
    }

    TEST_CASE("Callbacks arrive children-first (grandchild before child before state)")
    {
        // ObjectUpdate walks children before the parent's own Update(), so grandchild
        // should increment before child, and child before state.
        auto root = std::make_shared<duin::GameObject>();
        auto sm = root->CreateChildObject<duin::GameStateMachine>();

        std::vector<std::string> order;

        auto stateHolder = sm->CreateState<TestStateA>();
        stateHolder->ConnectOnStateUpdate([&order](double) { order.push_back("state"); });

        auto child = stateHolder->CreateChildObject<TrackingObject>();
        child->ConnectOnObjectUpdate([&order](double) { order.push_back("child"); });

        auto grandchild = child->CreateChildObject<TrackingObject>();
        grandchild->ConnectOnObjectUpdate([&order](double) { order.push_back("grandchild"); });

        sm->PushState(stateHolder);
        root->ObjectUpdate(0.016);

        REQUIRE(order.size() == 3);
        MSG_CHECK(order[0], order[0] == "grandchild");
        MSG_CHECK(order[1], order[1] == "child");
        MSG_CHECK(order[2], order[2] == "state");
    }

    TEST_CASE("Paused state and its children receive no callbacks while another state is on top")
    {
        // PushState pauses the current top. The paused state's children must not be called.
        auto root = std::make_shared<duin::GameObject>();
        auto sm = root->CreateChildObject<duin::GameStateMachine>();

        auto *stateA = sm->PushState<TestStateA>();
        auto childA = stateA->CreateChildObject<TrackingObject>();

        // Push stateB on top — stateA is now paused
        auto *stateB = sm->PushState<TestStateA>();
        auto childB = stateB->CreateChildObject<TrackingObject>();

        root->ObjectUpdate(0.016);
        root->ObjectPhysicsUpdate(0.016);
        root->ObjectDraw();
        root->ObjectDrawUI();
        root->ObjectOnEvent(duin::Event{});

        // stateB (top) and its child should receive callbacks
        MSG_CHECK(stateB->updateCalls, stateB->updateCalls == 1);
        MSG_CHECK(stateB->physicsUpdateCalls, stateB->physicsUpdateCalls == 1);
        MSG_CHECK(stateB->drawCalls, stateB->drawCalls == 1);
        MSG_CHECK(stateB->drawUICalls, stateB->drawUICalls == 1);
        MSG_CHECK(stateB->eventCalls, stateB->eventCalls == 1);

        MSG_CHECK(childB->updateCalls, childB->updateCalls == 1);
        MSG_CHECK(childB->physicsUpdateCalls, childB->physicsUpdateCalls == 1);
        MSG_CHECK(childB->drawCalls, childB->drawCalls == 1);
        MSG_CHECK(childB->drawUICalls, childB->drawUICalls == 1);
        MSG_CHECK(childB->eventCalls, childB->eventCalls == 1);

        // stateA (paused) and its child must NOT receive callbacks
        MSG_CHECK(stateA->updateCalls, stateA->updateCalls == 0);
        MSG_CHECK(stateA->physicsUpdateCalls, stateA->physicsUpdateCalls == 0);
        MSG_CHECK(stateA->drawCalls, stateA->drawCalls == 0);
        MSG_CHECK(stateA->drawUICalls, stateA->drawUICalls == 0);
        MSG_CHECK(stateA->eventCalls, stateA->eventCalls == 0);

        MSG_CHECK(childA->updateCalls, childA->updateCalls == 0);
        MSG_CHECK(childA->physicsUpdateCalls, childA->physicsUpdateCalls == 0);
        MSG_CHECK(childA->drawCalls, childA->drawCalls == 0);
        MSG_CHECK(childA->drawUICalls, childA->drawUICalls == 0);
        MSG_CHECK(childA->eventCalls, childA->eventCalls == 0);
    }

    TEST_CASE("Popped state's children receive no callbacks after pop")
    {
        auto root = std::make_shared<duin::GameObject>();
        auto sm = root->CreateChildObject<duin::GameStateMachine>();

        auto *stateA = sm->PushState<TestStateA>();
        std::weak_ptr<TrackingObject> childA = stateA->CreateChildObject<TrackingObject>();

        sm->PushState<TestStateA>(); // push stateB on top
        sm->PopState();              // pop stateB, stateA resumes

        stateA->updateCalls = 0;
        childA.lock()->updateCalls = 0;

        root->ObjectUpdate(0.016);
        root->ObjectPhysicsUpdate(0.016);
        root->ObjectDraw();
        root->ObjectDrawUI();
        root->ObjectOnEvent(duin::Event{});

        // stateA resumed — it and its child should receive callbacks
        MSG_CHECK(stateA->updateCalls, stateA->updateCalls == 1);
        MSG_CHECK(stateA->physicsUpdateCalls, stateA->physicsUpdateCalls == 1);
        MSG_CHECK(stateA->drawCalls, stateA->drawCalls == 1);
        MSG_CHECK(stateA->drawUICalls, stateA->drawUICalls == 1);
        MSG_CHECK(stateA->eventCalls, stateA->eventCalls == 1);

        MSG_CHECK(childA.lock()->updateCalls, childA.lock()->updateCalls == 1);
        MSG_CHECK(childA.lock()->physicsUpdateCalls, childA.lock()->physicsUpdateCalls == 1);
        MSG_CHECK(childA.lock()->drawCalls, childA.lock()->drawCalls == 1);
        MSG_CHECK(childA.lock()->drawUICalls, childA.lock()->drawUICalls == 1);
        MSG_CHECK(childA.lock()->eventCalls, childA.lock()->eventCalls == 1);
    }

    TEST_CASE("Disabled child of active state receives no callbacks")
    {
        auto root = std::make_shared<duin::GameObject>();
        auto sm = root->CreateChildObject<duin::GameStateMachine>();
        auto *state = sm->PushState<TestStateA>();

        auto child = state->CreateChildObject<TrackingObject>();
        child->Enable(false);

        root->ObjectUpdate(0.016);
        root->ObjectPhysicsUpdate(0.016);
        root->ObjectDraw();
        root->ObjectDrawUI();
        root->ObjectOnEvent(duin::Event{});

        MSG_CHECK(child->updateCalls, child->updateCalls == 0);
        MSG_CHECK(child->physicsUpdateCalls, child->physicsUpdateCalls == 0);
        MSG_CHECK(child->drawCalls, child->drawCalls == 0);
        MSG_CHECK(child->drawUICalls, child->drawUICalls == 0);
        MSG_CHECK(child->eventCalls, child->eventCalls == 0);
    }

    TEST_CASE("Re-enabling a child of active state resumes callbacks")
    {
        auto root = std::make_shared<duin::GameObject>();
        auto sm = root->CreateChildObject<duin::GameStateMachine>();
        auto *state = sm->PushState<TestStateA>();

        auto child = state->CreateChildObject<TrackingObject>();
        child->Enable(false);

        root->ObjectUpdate(0.016);
        MSG_CHECK(child->updateCalls, child->updateCalls == 0);

        child->Enable(true);
        root->ObjectUpdate(0.016);
        MSG_CHECK(child->updateCalls, child->updateCalls == 1);
    }

    TEST_CASE("Children of state are not updated when sm has no active state")
    {
        // After FlushStack the sm has no active state; no callbacks should propagate.
        auto root = std::make_shared<duin::GameObject>();
        auto sm = root->CreateChildObject<duin::GameStateMachine>();

        auto stateHolder = sm->CreateState<TestStateA>();
        auto child = stateHolder->CreateChildObject<TrackingObject>();
        sm->PushState(stateHolder);

        sm->FlushStack();
        child->updateCalls = 0;

        root->ObjectUpdate(0.016);
        root->ObjectPhysicsUpdate(0.016);
        root->ObjectDraw();
        root->ObjectDrawUI();
        root->ObjectOnEvent(duin::Event{});

        MSG_CHECK(child->updateCalls, child->updateCalls == 0);
        MSG_CHECK(child->physicsUpdateCalls, child->physicsUpdateCalls == 0);
        MSG_CHECK(child->drawCalls, child->drawCalls == 0);
        MSG_CHECK(child->drawUICalls, child->drawUICalls == 0);
        MSG_CHECK(child->eventCalls, child->eventCalls == 0);
    }
}

TEST_SUITE("GameStateMachine - Paused State Isolation")
{
    TEST_CASE("Paused state receives no callbacks via sm->Update directly")
    {
        // Verify isolation holds even when calling sm->Update() directly,
        // not just through root->ObjectUpdate().
        auto sm = std::make_shared<duin::GameStateMachine>();

        auto *stateA = sm->PushState<TestStateA>();
        auto childA  = stateA->CreateChildObject<TrackingObject>();
        auto *stateB = sm->PushState<TestStateA>();
        auto childB  = stateB->CreateChildObject<TrackingObject>();

        sm->Update(0.016);
        sm->PhysicsUpdate(0.016);
        sm->Draw();
        sm->DrawUI();
        sm->OnEvent(duin::Event{});

        MSG_CHECK(stateB->updateCalls,        stateB->updateCalls == 1);
        MSG_CHECK(stateB->physicsUpdateCalls, stateB->physicsUpdateCalls == 1);
        MSG_CHECK(stateB->drawCalls,          stateB->drawCalls == 1);
        MSG_CHECK(stateB->drawUICalls,        stateB->drawUICalls == 1);
        MSG_CHECK(stateB->eventCalls,         stateB->eventCalls == 1);

        MSG_CHECK(childB->updateCalls,        childB->updateCalls == 1);
        MSG_CHECK(childB->physicsUpdateCalls, childB->physicsUpdateCalls == 1);
        MSG_CHECK(childB->drawCalls,          childB->drawCalls == 1);
        MSG_CHECK(childB->drawUICalls,        childB->drawUICalls == 1);
        MSG_CHECK(childB->eventCalls,         childB->eventCalls == 1);

        MSG_CHECK(stateA->updateCalls,        stateA->updateCalls == 0);
        MSG_CHECK(stateA->physicsUpdateCalls, stateA->physicsUpdateCalls == 0);
        MSG_CHECK(stateA->drawCalls,          stateA->drawCalls == 0);
        MSG_CHECK(stateA->drawUICalls,        stateA->drawUICalls == 0);
        MSG_CHECK(stateA->eventCalls,         stateA->eventCalls == 0);

        MSG_CHECK(childA->updateCalls,        childA->updateCalls == 0);
        MSG_CHECK(childA->physicsUpdateCalls, childA->physicsUpdateCalls == 0);
        MSG_CHECK(childA->drawCalls,          childA->drawCalls == 0);
        MSG_CHECK(childA->drawUICalls,        childA->drawUICalls == 0);
        MSG_CHECK(childA->eventCalls,         childA->eventCalls == 0);
    }

    TEST_CASE("Three-deep stack: only top state and its children receive callbacks")
    {
        // Push A, then B on top of A, then C on top of B.
        // Only C (top) and its child should receive callbacks; A and B are paused.
        auto root = std::make_shared<duin::GameObject>();
        auto sm   = root->CreateChildObject<duin::GameStateMachine>();

        auto *stateA = sm->PushState<TestStateA>();
        auto childA  = stateA->CreateChildObject<TrackingObject>();

        auto *stateB = sm->PushState<TestStateA>();
        auto childB  = stateB->CreateChildObject<TrackingObject>();

        auto *stateC = sm->PushState<TestStateA>();
        auto childC  = stateC->CreateChildObject<TrackingObject>();

        root->ObjectUpdate(0.016);
        root->ObjectPhysicsUpdate(0.016);
        root->ObjectDraw();
        root->ObjectDrawUI();
        root->ObjectOnEvent(duin::Event{});

        MSG_CHECK(stateC->updateCalls,        stateC->updateCalls == 1);
        MSG_CHECK(stateC->physicsUpdateCalls, stateC->physicsUpdateCalls == 1);
        MSG_CHECK(stateC->drawCalls,          stateC->drawCalls == 1);
        MSG_CHECK(stateC->drawUICalls,        stateC->drawUICalls == 1);
        MSG_CHECK(stateC->eventCalls,         stateC->eventCalls == 1);

        MSG_CHECK(childC->updateCalls,        childC->updateCalls == 1);
        MSG_CHECK(childC->physicsUpdateCalls, childC->physicsUpdateCalls == 1);
        MSG_CHECK(childC->drawCalls,          childC->drawCalls == 1);
        MSG_CHECK(childC->drawUICalls,        childC->drawUICalls == 1);
        MSG_CHECK(childC->eventCalls,         childC->eventCalls == 1);

        MSG_CHECK(stateB->updateCalls,        stateB->updateCalls == 0);
        MSG_CHECK(stateB->physicsUpdateCalls, stateB->physicsUpdateCalls == 0);
        MSG_CHECK(stateB->drawCalls,          stateB->drawCalls == 0);
        MSG_CHECK(stateB->drawUICalls,        stateB->drawUICalls == 0);
        MSG_CHECK(stateB->eventCalls,         stateB->eventCalls == 0);

        MSG_CHECK(childB->updateCalls,        childB->updateCalls == 0);
        MSG_CHECK(childB->physicsUpdateCalls, childB->physicsUpdateCalls == 0);
        MSG_CHECK(childB->drawCalls,          childB->drawCalls == 0);
        MSG_CHECK(childB->drawUICalls,        childB->drawUICalls == 0);
        MSG_CHECK(childB->eventCalls,         childB->eventCalls == 0);

        MSG_CHECK(stateA->updateCalls,        stateA->updateCalls == 0);
        MSG_CHECK(stateA->physicsUpdateCalls, stateA->physicsUpdateCalls == 0);
        MSG_CHECK(stateA->drawCalls,          stateA->drawCalls == 0);
        MSG_CHECK(stateA->drawUICalls,        stateA->drawUICalls == 0);
        MSG_CHECK(stateA->eventCalls,         stateA->eventCalls == 0);

        MSG_CHECK(childA->updateCalls,        childA->updateCalls == 0);
        MSG_CHECK(childA->physicsUpdateCalls, childA->physicsUpdateCalls == 0);
        MSG_CHECK(childA->drawCalls,          childA->drawCalls == 0);
        MSG_CHECK(childA->drawUICalls,        childA->drawUICalls == 0);
        MSG_CHECK(childA->eventCalls,         childA->eventCalls == 0);
    }

    TEST_CASE("Popping top state resumes the state below and its children")
    {
        // Push A then B. Pop B — A resumes and its children should receive callbacks again.
        auto root = std::make_shared<duin::GameObject>();
        auto sm   = root->CreateChildObject<duin::GameStateMachine>();

        auto *stateA = sm->PushState<TestStateA>();
        auto childA  = stateA->CreateChildObject<TrackingObject>();

        sm->PushState<TestStateA>(); // stateB on top, stateA paused
        sm->PopState();              // stateB popped, stateA resumes

        // Reset any counts that accrued before the pop
        stateA->updateCalls = 0; stateA->physicsUpdateCalls = 0;
        stateA->drawCalls   = 0; stateA->drawUICalls = 0; stateA->eventCalls = 0;
        childA->updateCalls = 0; childA->physicsUpdateCalls = 0;
        childA->drawCalls   = 0; childA->drawUICalls = 0; childA->eventCalls = 0;

        root->ObjectUpdate(0.016);
        root->ObjectPhysicsUpdate(0.016);
        root->ObjectDraw();
        root->ObjectDrawUI();
        root->ObjectOnEvent(duin::Event{});

        MSG_CHECK(stateA->updateCalls,        stateA->updateCalls == 1);
        MSG_CHECK(stateA->physicsUpdateCalls, stateA->physicsUpdateCalls == 1);
        MSG_CHECK(stateA->drawCalls,          stateA->drawCalls == 1);
        MSG_CHECK(stateA->drawUICalls,        stateA->drawUICalls == 1);
        MSG_CHECK(stateA->eventCalls,         stateA->eventCalls == 1);

        MSG_CHECK(childA->updateCalls,        childA->updateCalls == 1);
        MSG_CHECK(childA->physicsUpdateCalls, childA->physicsUpdateCalls == 1);
        MSG_CHECK(childA->drawCalls,          childA->drawCalls == 1);
        MSG_CHECK(childA->drawUICalls,        childA->drawUICalls == 1);
        MSG_CHECK(childA->eventCalls,         childA->eventCalls == 1);
    }

    TEST_CASE("Popping two states from a three-deep stack resumes bottom state and its children")
    {
        auto root = std::make_shared<duin::GameObject>();
        auto sm   = root->CreateChildObject<duin::GameStateMachine>();

        auto *stateA = sm->PushState<TestStateA>();
        auto childA  = stateA->CreateChildObject<TrackingObject>();

        sm->PushState<TestStateA>(); // stateB, stateA paused
        sm->PushState<TestStateA>(); // stateC on top, B and A paused

        sm->PopState(); // pop C, B resumes
        sm->PopState(); // pop B, A resumes

        stateA->updateCalls = 0; stateA->physicsUpdateCalls = 0;
        stateA->drawCalls   = 0; stateA->drawUICalls = 0; stateA->eventCalls = 0;
        childA->updateCalls = 0; childA->physicsUpdateCalls = 0;
        childA->drawCalls   = 0; childA->drawUICalls = 0; childA->eventCalls = 0;

        root->ObjectUpdate(0.016);
        root->ObjectPhysicsUpdate(0.016);
        root->ObjectDraw();
        root->ObjectDrawUI();
        root->ObjectOnEvent(duin::Event{});

        MSG_CHECK(stateA->updateCalls,        stateA->updateCalls == 1);
        MSG_CHECK(stateA->physicsUpdateCalls, stateA->physicsUpdateCalls == 1);
        MSG_CHECK(stateA->drawCalls,          stateA->drawCalls == 1);
        MSG_CHECK(stateA->drawUICalls,        stateA->drawUICalls == 1);
        MSG_CHECK(stateA->eventCalls,         stateA->eventCalls == 1);

        MSG_CHECK(childA->updateCalls,        childA->updateCalls == 1);
        MSG_CHECK(childA->physicsUpdateCalls, childA->physicsUpdateCalls == 1);
        MSG_CHECK(childA->drawCalls,          childA->drawCalls == 1);
        MSG_CHECK(childA->drawUICalls,        childA->drawUICalls == 1);
        MSG_CHECK(childA->eventCalls,         childA->eventCalls == 1);
    }

    TEST_CASE("Paused state accumulates no calls across multiple frames")
    {
        auto root = std::make_shared<duin::GameObject>();
        auto sm   = root->CreateChildObject<duin::GameStateMachine>();

        auto *stateA = sm->PushState<TestStateA>();
        auto childA  = stateA->CreateChildObject<TrackingObject>();
        sm->PushState<TestStateA>(); // stateA now paused

        for (int i = 0; i < 5; ++i)
        {
            root->ObjectUpdate(0.016);
            root->ObjectPhysicsUpdate(0.016);
            root->ObjectDraw();
            root->ObjectDrawUI();
            root->ObjectOnEvent(duin::Event{});
        }

        MSG_CHECK(stateA->updateCalls,        stateA->updateCalls == 0);
        MSG_CHECK(stateA->physicsUpdateCalls, stateA->physicsUpdateCalls == 0);
        MSG_CHECK(stateA->drawCalls,          stateA->drawCalls == 0);
        MSG_CHECK(stateA->drawUICalls,        stateA->drawUICalls == 0);
        MSG_CHECK(stateA->eventCalls,         stateA->eventCalls == 0);

        MSG_CHECK(childA->updateCalls,        childA->updateCalls == 0);
        MSG_CHECK(childA->physicsUpdateCalls, childA->physicsUpdateCalls == 0);
        MSG_CHECK(childA->drawCalls,          childA->drawCalls == 0);
        MSG_CHECK(childA->drawUICalls,        childA->drawUICalls == 0);
        MSG_CHECK(childA->eventCalls,         childA->eventCalls == 0);
    }

    TEST_CASE("Resumed state after pop receives exactly one call per frame, not accumulated missed calls")
    {
        auto root = std::make_shared<duin::GameObject>();
        auto sm   = root->CreateChildObject<duin::GameStateMachine>();

        auto *stateA = sm->PushState<TestStateA>();
        auto childA  = stateA->CreateChildObject<TrackingObject>();
        sm->PushState<TestStateA>(); // stateA paused

        // Run 5 frames while stateA is paused
        for (int i = 0; i < 5; ++i)
            root->ObjectUpdate(0.016);

        sm->PopState(); // stateA resumes

        stateA->updateCalls = 0;
        childA->updateCalls = 0;

        // Run exactly 3 frames after resume
        for (int i = 0; i < 3; ++i)
            root->ObjectUpdate(0.016);

        // Should have exactly 3 calls, not 3+5
        MSG_CHECK(stateA->updateCalls, stateA->updateCalls == 3);
        MSG_CHECK(childA->updateCalls, childA->updateCalls == 3);
    }
}

} // namespace TestGameStateMachine
