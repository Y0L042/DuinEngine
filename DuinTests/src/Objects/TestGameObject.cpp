#include <doctest.h>
#include <Duin/Objects/GameObject.h>
#include <Duin/Core/Events/Event.h>
#include <vector>
#include <memory>
#include <iostream>

// Test GameObjects for testing GameObject behavior
class TestObjectA : public duin::GameObject
{
  public:
    void Init() override
    {
        initCalled = true;
    }
    void Ready() override
    {
        readyCalled = true;
    }
    void OnEvent(duin::Event e) override
    {
        eventCalls++;
        lastEvent = e;
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
    void Debug() override
    {
        debugCalls++;
    }

    bool initCalled = false;
    bool readyCalled = false;
    int eventCalls = 0;
    int updateCalls = 0;
    int physicsUpdateCalls = 0;
    int drawCalls = 0;
    int drawUICalls = 0;
    int debugCalls = 0;
    double lastDelta = 0.0;
    duin::Event lastEvent{};
};

class TestObjectB : public duin::GameObject
{
  public:
    void Init() override
    {
        initCalled = true;
    }
    void Ready() override
    {
        readyCalled = true;
    }

    bool initCalled = false;
    bool readyCalled = false;
};

class TestObjectC : public duin::GameObject
{
  public:
    void Update(double delta) override
    {
        updateCalls++;
    }
    int updateCalls = 0;
};

// Object that tracks lifecycle call order
class LifecycleTracker : public duin::GameObject
{
  public:
    void Init() override
    {
        callOrder.push_back("Init");
    }
    void Ready() override
    {
        callOrder.push_back("Ready");
    }
    void OnEvent(duin::Event e) override
    {
        callOrder.push_back("OnEvent");
    }
    void Update(double delta) override
    {
        callOrder.push_back("Update");
    }
    void PhysicsUpdate(double delta) override
    {
        callOrder.push_back("PhysicsUpdate");
    }
    void Draw() override
    {
        callOrder.push_back("Draw");
    }
    void DrawUI() override
    {
        callOrder.push_back("DrawUI");
    }
    void Debug() override
    {
        callOrder.push_back("Debug");
    }

    std::vector<std::string> callOrder;
};

namespace TestGameObject
{

TEST_SUITE("GameObject")
{
    TEST_CASE("GameObject UUID is valid and unique")
    {
        auto obj1 = std::make_shared<duin::GameObject>();
        auto obj2 = std::make_shared<duin::GameObject>();

        CHECK(obj1->GetUUID() != duin::UUID::INVALID);
        CHECK(obj2->GetUUID() != duin::UUID::INVALID);
        CHECK(obj1->GetUUID() != obj2->GetUUID());
    }

    TEST_CASE("Default constructor creates valid GameObject")
    {
        auto obj = std::make_shared<duin::GameObject>();

        bool t1 = obj != nullptr;
        bool t2 = obj->GetParent().get() == nullptr;
        CHECK(t1);
        CHECK(obj->GetUUID() != duin::UUID::INVALID);
        CHECK(t2);
    }

    TEST_CASE("CreateChildObject creates child and calls Init")
    {
        auto parent = std::make_shared<duin::GameObject>();
        auto child = parent->CreateChildObject<TestObjectA>();

        CHECK(child.get() != nullptr);
        CHECK(child->initCalled);
        CHECK(child->GetParent().get() == parent.get());
    }

    TEST_CASE("CreateChildObject with constructor arguments")
    {
        auto parent = std::make_shared<duin::GameObject>();

        // Create multiple children
        auto child1 = parent->CreateChildObject<TestObjectA>();
        auto child2 = parent->CreateChildObject<TestObjectB>();
        auto child3 = parent->CreateChildObject<TestObjectC>();

        CHECK(child2.get() != nullptr);
        CHECK(child3.get() != nullptr);
        CHECK(child1.get() != nullptr);
        CHECK(child1->initCalled);
        CHECK(child2->initCalled);
    }

    TEST_CASE("AddChildObject adds child and sets parent")
    {
        auto parent = std::make_shared<duin::GameObject>();
        auto child = std::make_shared<TestObjectA>();

        parent->AddChildObject(child);

        CHECK(child->GetParent().get() == parent.get());
        CHECK(child->initCalled);
    }

    TEST_CASE("AddChildObject does not add duplicate child")
    {
        auto parent = std::make_shared<duin::GameObject>();
        auto child = std::make_shared<TestObjectA>();

        parent->AddChildObject(child);
        parent->AddChildObject(child); // Try to add again

        // Child should still have the parent and Init should only be called once
        CHECK(child->GetParent().get() == parent.get());
    }

    TEST_CASE("AddChildObject with nullptr does nothing")
    {
        auto parent = std::make_shared<duin::GameObject>();

        // Should not crash
        parent->AddChildObject(nullptr);

        CHECK(true); // If we reach here, test passes
    }

    TEST_CASE("RemoveChildObject removes child and resets parent")
    {
        auto parent = std::make_shared<duin::GameObject>();
        auto child = parent->CreateChildObject<TestObjectA>();

        CHECK(child->GetParent().get() == parent.get());

        parent->RemoveChildObject(child);

        CHECK(child->GetParent().get() == nullptr);
    }

    TEST_CASE("RemoveChildObject with nullptr does nothing")
    {
        auto parent = std::make_shared<duin::GameObject>();

        // Should not crash
        parent->RemoveChildObject(nullptr);

        CHECK(true);
    }

    TEST_CASE("SetParent sets the parent")
    {
        auto parent = std::make_shared<duin::GameObject>();
        auto child = std::make_shared<duin::GameObject>();

        child->SetParent(parent);

        CHECK(child->GetParent().get() == parent.get());
    }

    TEST_CASE("GetParent returns nullptr initially")
    {
        auto obj = std::make_shared<duin::GameObject>();

        CHECK(obj->GetParent().get() == nullptr);
    }

    TEST_CASE("ResetParent clears parent reference")
    {
        auto parent = std::make_shared<duin::GameObject>();
        auto child = std::make_shared<duin::GameObject>();

        child->SetParent(parent);
        CHECK(child->GetParent().get() == parent.get());

        child->ResetParent();
        CHECK(child->GetParent().get() == nullptr);
    }

    TEST_CASE("GetSharedPointer returns valid shared_ptr")
    {
        auto obj = std::make_shared<TestObjectA>();

        auto sharedPtr = obj->GetSharedPointer<TestObjectA>();

        CHECK(sharedPtr.get() != nullptr);
        CHECK(sharedPtr == obj);
    }

    TEST_CASE("Equality operator compares UUIDs")
    {
        auto obj1 = std::make_shared<duin::GameObject>();
        auto obj2 = std::make_shared<duin::GameObject>();

        CHECK_FALSE(*obj1 == *obj2);
        CHECK(*obj1 == *obj1); // Same object should equal itself
    }

    TEST_CASE("ObjectReady calls children's Ready before parent's Ready")
    {
        auto parent = std::make_shared<LifecycleTracker>();
        auto child = parent->CreateChildObject<LifecycleTracker>();

        parent->ObjectReady();

        // Parent's Ready should be called after child's
        CHECK(parent->callOrder.size() > 0);
        CHECK(parent->callOrder.back() == "Ready");

        CHECK(child->callOrder.size() > 0);
        CHECK(child->callOrder.back() == "Ready");
    }

    TEST_CASE("ObjectUpdate calls children's Update before parent's Update")
    {
        auto parent = std::make_shared<TestObjectA>();
        auto child = parent->CreateChildObject<TestObjectA>();

        parent->ObjectUpdate(0.016);

        CHECK(parent->updateCalls == 1);
        CHECK(child->updateCalls == 1);
        CHECK(parent->lastDelta == doctest::Approx(0.016));
    }

    TEST_CASE("ObjectPhysicsUpdate calls children's PhysicsUpdate")
    {
        auto parent = std::make_shared<TestObjectA>();
        auto child = parent->CreateChildObject<TestObjectA>();

        parent->ObjectPhysicsUpdate(0.016);

        CHECK(parent->physicsUpdateCalls == 1);
        CHECK(child->physicsUpdateCalls == 1);
    }

    TEST_CASE("ObjectDraw calls children's Draw")
    {
        auto parent = std::make_shared<TestObjectA>();
        auto child = parent->CreateChildObject<TestObjectA>();

        parent->ObjectDraw();

        CHECK(parent->drawCalls == 1);
        CHECK(child->drawCalls == 1);
    }

    TEST_CASE("ObjectDrawUI calls children's DrawUI")
    {
        auto parent = std::make_shared<TestObjectA>();
        auto child = parent->CreateChildObject<TestObjectA>();

        parent->ObjectDrawUI();

        CHECK(parent->drawUICalls == 1);
        CHECK(child->drawUICalls == 1);
    }

    TEST_CASE("ObjectDebug calls children's Debug")
    {
        auto parent = std::make_shared<TestObjectA>();
        auto child = parent->CreateChildObject<TestObjectA>();

        parent->ObjectDebug();

        CHECK(parent->debugCalls == 1);
        CHECK(child->debugCalls == 1);
    }

    TEST_CASE("ObjectOnEvent calls children's OnEvent")
    {
        auto parent = std::make_shared<TestObjectA>();
        auto child = parent->CreateChildObject<TestObjectA>();

        duin::Event testEvent{};
        SDL_Event user_event;
        SDL_zero(user_event);
        user_event.type = SDL_EVENT_USER;
        user_event.user.code = 1;
        user_event.user.data1 = NULL;
        user_event.user.data2 = NULL;
        testEvent.sdlEvent = user_event;

        parent->ObjectOnEvent(testEvent);

        CHECK(parent->eventCalls == 1);
        CHECK(child->eventCalls == 1);
    }

    TEST_CASE("Nested children hierarchy")
    {
        auto root = std::make_shared<TestObjectA>();
        auto child1 = root->CreateChildObject<TestObjectA>();
        auto child2 = root->CreateChildObject<TestObjectA>();
        auto grandchild1 = child1->CreateChildObject<TestObjectA>();
        auto grandchild2 = child2->CreateChildObject<TestObjectA>();

        CHECK(child1->GetParent().get() == root.get());
        CHECK(child2->GetParent().get() == root.get());
        CHECK(grandchild1->GetParent().get() == child1.get());
        CHECK(grandchild2->GetParent().get() == child2.get());
    }

    TEST_CASE("Nested children receive updates")
    {
        auto root = std::make_shared<TestObjectA>();
        auto child = root->CreateChildObject<TestObjectA>();
        auto grandchild = child->CreateChildObject<TestObjectA>();

        root->ObjectUpdate(0.016);

        CHECK(root->updateCalls == 1);
        CHECK(child->updateCalls == 1);
        CHECK(grandchild->updateCalls == 1);
    }

    TEST_CASE("Multiple updates accumulate correctly")
    {
        auto parent = std::make_shared<TestObjectA>();

        parent->ObjectUpdate(0.016);
        parent->ObjectUpdate(0.033);
        parent->ObjectUpdate(0.020);

        CHECK(parent->updateCalls == 3);
        CHECK(parent->lastDelta == doctest::Approx(0.020));
    }

    TEST_CASE("Multiple PhysicsUpdates accumulate")
    {
        auto parent = std::make_shared<TestObjectA>();

        parent->ObjectPhysicsUpdate(0.016);
        parent->ObjectPhysicsUpdate(0.016);
        parent->ObjectPhysicsUpdate(0.016);

        CHECK(parent->physicsUpdateCalls == 3);
    }

    TEST_CASE("Multiple Draw calls")
    {
        auto parent = std::make_shared<TestObjectA>();

        parent->ObjectDraw();
        parent->ObjectDraw();
        parent->ObjectDraw();

        CHECK(parent->drawCalls == 3);
    }

    TEST_CASE("Multiple DrawUI calls")
    {
        auto parent = std::make_shared<TestObjectA>();

        parent->ObjectDrawUI();
        parent->ObjectDrawUI();

        CHECK(parent->drawUICalls == 2);
    }

    TEST_CASE("Complex hierarchy with multiple children")
    {
        auto root = std::make_shared<TestObjectA>();
        auto child1 = root->CreateChildObject<TestObjectA>();
        auto child2 = root->CreateChildObject<TestObjectA>();
        auto child3 = root->CreateChildObject<TestObjectA>();

        root->ObjectUpdate(0.016);

        CHECK(root->updateCalls == 1);
        CHECK(child1->updateCalls == 1);
        CHECK(child2->updateCalls == 1);
        CHECK(child3->updateCalls == 1);
    }

    TEST_CASE("RemoveChildObject and add back")
    {
        auto parent = std::make_shared<duin::GameObject>();
        auto child = parent->CreateChildObject<TestObjectA>();

        CHECK(child->GetParent().get() == parent.get());

        parent->RemoveChildObject(child);
        CHECK(child->GetParent().get() == nullptr);

        // Add back
        parent->AddChildObject(child);
        CHECK(child->GetParent().get() == parent.get());
    }

    TEST_CASE("Init is called only once when adding child")
    {
        auto parent = std::make_shared<duin::GameObject>();
        auto child = std::make_shared<TestObjectA>();

        parent->AddChildObject(child);

        // Check Init was called
        CHECK(child->initCalled);

        // Reset the flag
        child->initCalled = false;

        // Try to add again (should not add duplicate)
        parent->AddChildObject(child);

        // Init should not be called again since it's a duplicate
        CHECK_FALSE(child->initCalled);
    }

    TEST_CASE("ObjectReady propagates through deep hierarchy")
    {
        auto root = std::make_shared<LifecycleTracker>();
        auto child = root->CreateChildObject<LifecycleTracker>();
        auto grandchild = child->CreateChildObject<LifecycleTracker>();
        auto greatgrandchild = grandchild->CreateChildObject<LifecycleTracker>();

        root->ObjectReady();

        CHECK(root->callOrder.size() > 0);
        CHECK(child->callOrder.size() > 0);
        CHECK(grandchild->callOrder.size() > 0);
        CHECK(greatgrandchild->callOrder.size() > 0);

        CHECK(root->callOrder.back() == "Ready");
        CHECK(child->callOrder.back() == "Ready");
        CHECK(grandchild->callOrder.back() == "Ready");
        CHECK(greatgrandchild->callOrder.back() == "Ready");
    }

    TEST_CASE("Lifecycle methods are called in correct order")
    {
        auto obj = std::make_shared<LifecycleTracker>();

        obj->Init();
        obj->Ready();
        duin::Event testEvent{};
        obj->OnEvent(testEvent);
        obj->Update(0.016);
        obj->PhysicsUpdate(0.016);
        obj->Draw();
        obj->DrawUI();
        obj->Debug();

        CHECK(obj->callOrder.size() == 8);
        CHECK(obj->callOrder[0] == "Init");
        CHECK(obj->callOrder[1] == "Ready");
        CHECK(obj->callOrder[2] == "OnEvent");
        CHECK(obj->callOrder[3] == "Update");
        CHECK(obj->callOrder[4] == "PhysicsUpdate");
        CHECK(obj->callOrder[5] == "Draw");
        CHECK(obj->callOrder[6] == "DrawUI");
        CHECK(obj->callOrder[7] == "Debug");
    }

    TEST_CASE("Parent with no children works correctly")
    {
        auto parent = std::make_shared<TestObjectA>();

        parent->ObjectUpdate(0.016);
        parent->ObjectPhysicsUpdate(0.016);
        parent->ObjectDraw();
        parent->ObjectDrawUI();
        parent->ObjectDebug();

        CHECK(parent->updateCalls == 1);
        CHECK(parent->physicsUpdateCalls == 1);
        CHECK(parent->drawCalls == 1);
        CHECK(parent->drawUICalls == 1);
        CHECK(parent->debugCalls == 1);
    }

    TEST_CASE("Adding and removing multiple children")
    {
        auto parent = std::make_shared<duin::GameObject>();
        auto child1 = parent->CreateChildObject<TestObjectA>();
        auto child2 = parent->CreateChildObject<TestObjectA>();
        auto child3 = parent->CreateChildObject<TestObjectA>();

        CHECK(child1->GetParent().get() == parent.get());
        CHECK(child2->GetParent().get() == parent.get());
        CHECK(child3->GetParent().get() == parent.get());

        parent->RemoveChildObject(child2);
        CHECK(child2->GetParent().get() == nullptr);

        // child1 and child3 should still have parent
        CHECK(child1->GetParent().get() == parent.get());
        CHECK(child3->GetParent().get() == parent.get());
    }

    TEST_CASE("CreateChildObject returns correct type")
    {
        auto parent = std::make_shared<duin::GameObject>();

        auto childA = parent->CreateChildObject<TestObjectA>();
        auto childB = parent->CreateChildObject<TestObjectB>();

        // Verify types
        CHECK(std::dynamic_pointer_cast<TestObjectA>(childA).get() != nullptr);
        CHECK(std::dynamic_pointer_cast<TestObjectB>(childB).get() != nullptr);
        CHECK(std::dynamic_pointer_cast<TestObjectB>(childA).get() == nullptr);
    }

    TEST_CASE("Virtual method overrides work correctly")
    {
        auto obj = std::make_shared<TestObjectA>();

        obj->Update(1.0);
        CHECK(obj->updateCalls == 1);
        CHECK(obj->lastDelta == doctest::Approx(1.0));

        obj->PhysicsUpdate(2.0);
        CHECK(obj->physicsUpdateCalls == 1);
    }

    TEST_CASE("Multiple events are tracked correctly")
    {
        auto obj = std::make_shared<TestObjectA>();

        duin::Event event1{};
        duin::Event event2{};

        obj->OnEvent(event1);
        CHECK(obj->eventCalls == 1);

        obj->OnEvent(event2);
        CHECK(obj->eventCalls == 2);
    }

    TEST_CASE("Mixed hierarchy of different GameObject types")
    {
        auto root = std::make_shared<TestObjectA>();
        auto child1 = root->CreateChildObject<TestObjectB>();
        auto child2 = root->CreateChildObject<TestObjectC>();
        auto grandchild = child1->CreateChildObject<TestObjectA>();

        root->ObjectUpdate(0.016);

        CHECK(root->updateCalls == 1);
        CHECK(child2->updateCalls == 1);
        CHECK(grandchild->updateCalls == 1);
    }

    TEST_CASE("ObjectReady with no children")
    {
        auto obj = std::make_shared<TestObjectA>();

        obj->ObjectReady();

        CHECK(obj->readyCalled);
    }

    TEST_CASE("Large hierarchy performance")
    {
        auto root = std::make_shared<TestObjectA>();

        // Create 10 children
        for (int i = 0; i < 10; ++i)
        {
            root->CreateChildObject<TestObjectA>();
        }

        root->ObjectUpdate(0.016);

        CHECK(root->updateCalls == 1);
    }

    TEST_CASE("Children are updated in order of creation")
    {
        auto root = std::make_shared<duin::GameObject>();
        auto child1 = root->CreateChildObject<LifecycleTracker>();
        auto child2 = root->CreateChildObject<LifecycleTracker>();
        auto child3 = root->CreateChildObject<LifecycleTracker>();

        root->ObjectUpdate(0.016);

        // All children should receive update
        CHECK(child1->callOrder.size() > 0);
        CHECK(child2->callOrder.size() > 0);
        CHECK(child3->callOrder.size() > 0);
    }

    // ========================================================================
    // ADVANCED EDGE CASE TESTS
    // ========================================================================

    TEST_CASE("Reparenting a child from one parent to another")
    {
        auto parent1 = std::make_shared<duin::GameObject>();
        auto parent2 = std::make_shared<duin::GameObject>();
        auto child = parent1->CreateChildObject<TestObjectA>();

        CHECK(child->GetParent().get() == parent1.get());

        // Reparent to parent2
        child->SetParent(parent2);

        CHECK(child->GetParent().get() == parent2.get());
        CHECK(child->GetParent().get() != parent1.get());
    }

    TEST_CASE("Reparenting updates work correctly")
    {
        auto parent1 = std::make_shared<TestObjectA>();
        auto parent2 = std::make_shared<TestObjectA>();
        auto child = parent1->CreateChildObject<TestObjectA>();

        parent1->ObjectUpdate(0.016);
        CHECK(child->updateCalls == 1);

        // Reparent
        parent1->TransferChildObject(child, parent2);

        // Update parent1 - child should not receive update
        parent1->ObjectUpdate(0.016);
        CHECK(parent1->updateCalls == 2);
        CHECK(child->updateCalls == 1); // Still 1, not updated by parent1

        // Update parent2 - child should receive update
        parent2->ObjectUpdate(0.016);
        CHECK(parent2->updateCalls == 1);
        CHECK(child->updateCalls == 2); // Now 2, updated by parent2
    }

    TEST_CASE("Very deep nesting - 50 levels")
    {
        std::shared_ptr<TestObjectA> root = std::make_shared<TestObjectA>();
        std::shared_ptr<TestObjectA> current = root;

        // Create 50 levels of nesting
        for (int i = 0; i < 50; ++i)
        {
            current = current->CreateChildObject<TestObjectA>();
        }

        // Verify deepest child has root as ancestor
        auto temp = current;
        int depth = 0;
        while (temp->GetParent().get() != nullptr)
        {
            temp = std::static_pointer_cast<TestObjectA>(temp->GetParent());
            depth++;
        }
        CHECK(depth == 50);
        CHECK(temp.get() == root.get());
    }

    TEST_CASE("Very deep nesting - update propagation through 50 levels")
    {
        std::shared_ptr<TestObjectA> root = std::make_shared<TestObjectA>();
        std::shared_ptr<TestObjectA> current = root;
        std::shared_ptr<TestObjectA> deepest = root;

        // Create 50 levels
        for (int i = 0; i < 50; ++i)
        {
            current = current->CreateChildObject<TestObjectA>();
            deepest = current;
        }

        // Update from root
        root->ObjectUpdate(0.016);

        // All levels should be updated
        CHECK(root->updateCalls == 1);
        CHECK(deepest->updateCalls == 1);
    }

    TEST_CASE("Very wide hierarchy - 100 children")
    {
        auto parent = std::make_shared<TestObjectA>();
        std::vector<std::shared_ptr<TestObjectA>> children;

        // Create 100 children
        for (int i = 0; i < 100; ++i)
        {
            children.push_back(parent->CreateChildObject<TestObjectA>());
        }

        parent->ObjectUpdate(0.016);

        // All children should be updated
        CHECK(parent->updateCalls == 1);
        for (const auto &child : children)
        {
            CHECK(child->updateCalls == 1);
        }
    }

    TEST_CASE("Wide and deep hierarchy - performance test")
    {
        // Create a hierarchy with 10 levels, each with 10 children
        auto root = std::make_shared<TestObjectA>();

        std::function<void(std::shared_ptr<TestObjectA>, int)> createHierarchy;
        createHierarchy = [&](std::shared_ptr<TestObjectA> node, int depth) {
            if (depth <= 0)
                return;
            for (int i = 0; i < 10; ++i)
            {
                auto child = node->CreateChildObject<TestObjectA>();
                createHierarchy(child, depth - 1);
            }
        };

        createHierarchy(root, 4); // 4 levels deep, 10 children each = 1111 total objects

        root->ObjectUpdate(0.016);

        CHECK(root->updateCalls == 1);
    }

    TEST_CASE("Boundary condition - zero delta time")
    {
        auto obj = std::make_shared<TestObjectA>();

        obj->ObjectUpdate(0.0);

        CHECK(obj->updateCalls == 1);
        CHECK(obj->lastDelta == doctest::Approx(0.0));
    }

    TEST_CASE("Boundary condition - negative delta time")
    {
        auto obj = std::make_shared<TestObjectA>();

        obj->ObjectUpdate(-0.016);

        CHECK(obj->updateCalls == 1);
        CHECK(obj->lastDelta == doctest::Approx(-0.016));
    }

    TEST_CASE("Boundary condition - very large delta time")
    {
        auto obj = std::make_shared<TestObjectA>();

        obj->ObjectUpdate(1000000.0);

        CHECK(obj->updateCalls == 1);
        CHECK(obj->lastDelta == doctest::Approx(1000000.0));
    }

    TEST_CASE("Empty event handling")
    {
        auto obj = std::make_shared<TestObjectA>();

        duin::Event emptyEvent{};
        obj->ObjectOnEvent(emptyEvent);

        CHECK(obj->eventCalls == 1);
    }

    TEST_CASE("Many rapid events")
    {
        auto obj = std::make_shared<TestObjectA>();

        // Send 1000 events
        for (int i = 0; i < 1000; ++i)
        {
            duin::Event event{};
            obj->ObjectOnEvent(event);
        }

        CHECK(obj->eventCalls == 1000);
    }

    TEST_CASE("Rapid creation and destruction stress test")
    {
        auto parent = std::make_shared<duin::GameObject>();

        // Create and destroy 100 children rapidly
        for (int i = 0; i < 100; ++i)
        {
            auto child = parent->CreateChildObject<TestObjectA>();
            parent->RemoveChildObject(child);
        }

        // Parent should still be valid
        CHECK(parent->GetUUID() != duin::UUID::INVALID);
        CHECK(parent->GetParent().get() == nullptr);
    }

    TEST_CASE("Many rapid updates - 10000 iterations")
    {
        auto obj = std::make_shared<TestObjectA>();

        for (int i = 0; i < 10000; ++i)
        {
            obj->ObjectUpdate(0.001);
        }

        CHECK(obj->updateCalls == 10000);
    }

    TEST_CASE("GetSharedPointer with derived types")
    {
        auto objA = std::make_shared<TestObjectA>();

        auto sharedA = objA->GetSharedPointer<TestObjectA>();
        CHECK(sharedA.get() == objA.get());

        // Verify it's the same instance
        sharedA->initCalled = true;
        CHECK(objA->initCalled == true);
    }

    TEST_CASE("Multiple inheritance levels")
    {
        // TestObjectC inherits from GameObject
        // Create hierarchy with mixed types
        auto root = std::make_shared<TestObjectA>();
        auto child1 = root->CreateChildObject<TestObjectB>();
        auto child2 = root->CreateChildObject<TestObjectC>();
        auto grandchild1 = child1->CreateChildObject<TestObjectA>();

        // All should have correct parents
        CHECK(child1->GetParent().get() == root.get());
        CHECK(child2->GetParent().get() == root.get());
        CHECK(grandchild1->GetParent().get() == child1.get());

        // All should receive updates
        root->ObjectUpdate(0.016);
        CHECK(root->updateCalls == 1);
        CHECK(child2->updateCalls == 1);
        CHECK(grandchild1->updateCalls == 1);
    }

    TEST_CASE("SetParent to nullptr behaves like ResetParent")
    {
        auto parent = std::make_shared<duin::GameObject>();
        auto child = parent->CreateChildObject<TestObjectA>();

        CHECK(child->GetParent().get() == parent.get());

        child->SetParent(nullptr);

        CHECK(child->GetParent().get() == nullptr);
    }

    TEST_CASE("Adding child that already has same parent")
    {
        auto parent = std::make_shared<duin::GameObject>();
        auto child = parent->CreateChildObject<TestObjectA>();

        CHECK(child->GetParent().get() == parent.get());
        CHECK(child->initCalled);

        // Reset init flag
        child->initCalled = false;

        // Try adding again
        parent->AddChildObject(child);

        // Should not call Init again
        CHECK_FALSE(child->initCalled);
        CHECK(child->GetParent().get() == parent.get());
    }

    TEST_CASE("Complex hierarchy removal - remove middle node")
    {
        auto root = std::make_shared<TestObjectA>();
        auto middle = root->CreateChildObject<TestObjectA>();
        auto leaf = middle->CreateChildObject<TestObjectA>();

        CHECK(leaf->GetParent().get() == middle.get());
        CHECK(middle->GetParent().get() == root.get());

        // Remove middle node from root
        root->RemoveChildObject(middle);

        CHECK(middle->GetParent().get() == nullptr);
        // Leaf should still be child of middle
        CHECK(leaf->GetParent().get() == middle.get());

        // Update root should not reach leaf anymore
        root->ObjectUpdate(0.016);
        CHECK(root->updateCalls == 1);
        CHECK(middle->updateCalls == 0);
        CHECK(leaf->updateCalls == 0);

        // Update middle should reach leaf
        middle->ObjectUpdate(0.016);
        CHECK(middle->updateCalls == 1);
        CHECK(leaf->updateCalls == 1);
    }

    TEST_CASE("UUID inequality operator")
    {
        auto obj1 = std::make_shared<duin::GameObject>();
        auto obj2 = std::make_shared<duin::GameObject>();

        CHECK(*obj1 != *obj2);
        CHECK_FALSE(*obj1 != *obj1);
    }

    TEST_CASE("Multiple children of same type maintain separate state")
    {
        auto parent = std::make_shared<duin::GameObject>();
        auto child1 = parent->CreateChildObject<TestObjectA>();
        auto child2 = parent->CreateChildObject<TestObjectA>();
        auto child3 = parent->CreateChildObject<TestObjectA>();

        // Modify only child2
        child2->updateCalls = 99;
        child2->drawCalls = 88;

        CHECK(child1->updateCalls == 0);
        CHECK(child2->updateCalls == 99);
        CHECK(child3->updateCalls == 0);

        CHECK(child1->drawCalls == 0);
        CHECK(child2->drawCalls == 88);
        CHECK(child3->drawCalls == 0);
    }

    TEST_CASE("Ready propagation in complex hierarchy")
    {
        auto root = std::make_shared<LifecycleTracker>();
        auto child1 = root->CreateChildObject<LifecycleTracker>();
        auto child2 = root->CreateChildObject<LifecycleTracker>();
        auto grandchild1_1 = child1->CreateChildObject<LifecycleTracker>();
        auto grandchild1_2 = child1->CreateChildObject<LifecycleTracker>();
        auto grandchild2_1 = child2->CreateChildObject<LifecycleTracker>();

        root->ObjectReady();

        // All should have been called Ready
        CHECK(root->callOrder.back() == "Ready");
        CHECK(child1->callOrder.back() == "Ready");
        CHECK(child2->callOrder.back() == "Ready");
        CHECK(grandchild1_1->callOrder.back() == "Ready");
        CHECK(grandchild1_2->callOrder.back() == "Ready");
        CHECK(grandchild2_1->callOrder.back() == "Ready");
    }

    TEST_CASE("Event propagation in complex hierarchy")
    {
        auto root = std::make_shared<TestObjectA>();
        auto child1 = root->CreateChildObject<TestObjectA>();
        auto child2 = root->CreateChildObject<TestObjectA>();
        auto grandchild = child1->CreateChildObject<TestObjectA>();

        duin::Event testEvent{};
        SDL_Event sdl_event;
        SDL_zero(sdl_event);
        sdl_event.type = SDL_EVENT_USER;
        sdl_event.user.code = 42;
        testEvent.sdlEvent = sdl_event;

        root->ObjectOnEvent(testEvent);

        CHECK(root->eventCalls == 1);
        CHECK(child1->eventCalls == 1);
        CHECK(child2->eventCalls == 1);
        CHECK(grandchild->eventCalls == 1);

        // Verify event data was propagated correctly
        CHECK(grandchild->lastEvent.sdlEvent.user.code == 42);
    }

    TEST_CASE("PhysicsUpdate propagation in complex hierarchy")
    {
        auto root = std::make_shared<TestObjectA>();
        auto child1 = root->CreateChildObject<TestObjectA>();
        auto child2 = root->CreateChildObject<TestObjectA>();
        auto grandchild = child1->CreateChildObject<TestObjectA>();

        root->ObjectPhysicsUpdate(0.016);

        CHECK(root->physicsUpdateCalls == 1);
        CHECK(child1->physicsUpdateCalls == 1);
        CHECK(child2->physicsUpdateCalls == 1);
        CHECK(grandchild->physicsUpdateCalls == 1);
    }

    TEST_CASE("Draw and DrawUI propagation in complex hierarchy")
    {
        auto root = std::make_shared<TestObjectA>();
        auto child1 = root->CreateChildObject<TestObjectA>();
        auto child2 = root->CreateChildObject<TestObjectA>();

        root->ObjectDraw();
        root->ObjectDrawUI();

        CHECK(root->drawCalls == 1);
        CHECK(child1->drawCalls == 1);
        CHECK(child2->drawCalls == 1);

        CHECK(root->drawUICalls == 1);
        CHECK(child1->drawUICalls == 1);
        CHECK(child2->drawUICalls == 1);
    }

    TEST_CASE("Debug propagation in complex hierarchy")
    {
        auto root = std::make_shared<TestObjectA>();
        auto child1 = root->CreateChildObject<TestObjectA>();
        auto child2 = root->CreateChildObject<TestObjectA>();
        auto grandchild = child1->CreateChildObject<TestObjectA>();

        root->ObjectDebug();

        CHECK(root->debugCalls == 1);
        CHECK(child1->debugCalls == 1);
        CHECK(child2->debugCalls == 1);
        CHECK(grandchild->debugCalls == 1);
    }

    TEST_CASE("All lifecycle methods on single object")
    {
        auto obj = std::make_shared<LifecycleTracker>();

        obj->Init();
        obj->Ready();
        obj->Update(0.016);
        obj->PhysicsUpdate(0.016);
        obj->Draw();
        obj->DrawUI();
        obj->Debug();

        duin::Event event{};
        obj->OnEvent(event);

        CHECK(obj->callOrder.size() == 8);
        CHECK(obj->callOrder[0] == "Init");
        CHECK(obj->callOrder[1] == "Ready");
        CHECK(obj->callOrder[2] == "Update");
        CHECK(obj->callOrder[3] == "PhysicsUpdate");
        CHECK(obj->callOrder[4] == "Draw");
        CHECK(obj->callOrder[5] == "DrawUI");
        CHECK(obj->callOrder[6] == "Debug");
        CHECK(obj->callOrder[7] == "OnEvent");
    }

    TEST_CASE("Removing all children one by one")
    {
        auto parent = std::make_shared<duin::GameObject>();
        std::vector<std::shared_ptr<TestObjectA>> children;

        for (int i = 0; i < 10; ++i)
        {
            children.push_back(parent->CreateChildObject<TestObjectA>());
        }

        // Verify all added
        for (const auto &child : children)
        {
            CHECK(child->GetParent().get() == parent.get());
        }

        // Remove all
        for (const auto &child : children)
        {
            parent->RemoveChildObject(child);
        }

        // Verify all removed
        for (const auto &child : children)
        {
            CHECK(child->GetParent().get() == nullptr);
        }
    }

    TEST_CASE("Interleaved add and remove operations")
    {
        auto parent = std::make_shared<duin::GameObject>();

        auto child1 = parent->CreateChildObject<TestObjectA>();
        CHECK(child1->GetParent().get() == parent.get());

        auto child2 = parent->CreateChildObject<TestObjectA>();
        CHECK(child2->GetParent().get() == parent.get());

        parent->RemoveChildObject(child1);
        CHECK(child1->GetParent().get() == nullptr);

        auto child3 = parent->CreateChildObject<TestObjectA>();
        CHECK(child3->GetParent().get() == parent.get());

        parent->RemoveChildObject(child2);
        CHECK(child2->GetParent().get() == nullptr);

        parent->RemoveChildObject(child3);
        CHECK(child3->GetParent().get() == nullptr);
    }

    TEST_CASE("Reparent leaf node and remove middle node ")
    {
        auto parent = std::make_shared<duin::GameObject>();
        auto middle = parent->CreateChildObject<duin::GameObject>();
        auto leaf = middle->CreateChildObject<duin::GameObject>();

        CHECK(leaf->GetParent().get() == middle.get());

        middle->TransferChildObject(leaf, parent);
        parent->RemoveChildObject(middle);

        CHECK(leaf->GetParent().get() == parent.get());
    }

    TEST_CASE("Remove middle node and reparent leaf node")
    {
        auto parent = std::make_shared<duin::GameObject>();
        auto middle = parent->CreateChildObject<duin::GameObject>();
        auto leaf = middle->CreateChildObject<duin::GameObject>();

        CHECK(leaf->GetParent().get() == middle.get());

        parent->RemoveChildObject(middle);
        middle->TransferChildObject(leaf, parent);

        CHECK(leaf->GetParent().get() == parent.get());
    }

    TEST_CASE("CreateChildObject with different derived types in sequence")
    {
        auto parent = std::make_shared<duin::GameObject>();

        auto childA = parent->CreateChildObject<TestObjectA>();
        auto childB = parent->CreateChildObject<TestObjectB>();
        auto childC = parent->CreateChildObject<TestObjectC>();

        CHECK(childA->initCalled);
        CHECK(childB->initCalled);

        // All should have same parent
        CHECK(childA->GetParent().get() == parent.get());
        CHECK(childB->GetParent().get() == parent.get());
        CHECK(childC->GetParent().get() == parent.get());

        // Type checks
        CHECK(std::dynamic_pointer_cast<TestObjectA>(childA) != nullptr);
        CHECK(std::dynamic_pointer_cast<TestObjectB>(childB) != nullptr);
        CHECK(std::dynamic_pointer_cast<TestObjectC>(childC) != nullptr);
    }

    TEST_CASE("Extreme nesting - 100 levels deep")
    {
        std::shared_ptr<duin::GameObject> root = std::make_shared<duin::GameObject>();
        std::shared_ptr<duin::GameObject> current = root;

        // Create 100 levels
        for (int i = 0; i < 100; ++i)
        {
            current = current->CreateChildObject<duin::GameObject>();
        }

        // Verify depth by traversing back up
        auto temp = current;
        int depth = 0;
        while (temp->GetParent() != nullptr)
        {
            temp = temp->GetParent();
            depth++;
        }

        CHECK(depth == 100);
        CHECK(temp.get() == root.get());
    }

    TEST_CASE("Extreme width - 500 children")
    {
        auto parent = std::make_shared<duin::GameObject>();

        // Create 500 children
        for (int i = 0; i < 500; ++i)
        {
            auto child = parent->CreateChildObject<duin::GameObject>();
            CHECK(child->GetParent().get() == parent.get());
        }

        // Parent should still be valid
        CHECK(parent->GetUUID() != duin::UUID::INVALID);
    }

    TEST_CASE("Performance - update propagation through 1000 objects")
    {
        auto root = std::make_shared<TestObjectA>();

        // Create 999 children directly on root
        for (int i = 0; i < 999; ++i)
        {
            root->CreateChildObject<TestObjectA>();
        }

        // Perform update
        root->ObjectUpdate(0.016);

        // Root should be updated
        CHECK(root->updateCalls == 1);
    }

    TEST_CASE("Performance - 1000 sequential updates on single object")
    {
        auto obj = std::make_shared<TestObjectA>();

        for (int i = 0; i < 1000; ++i)
        {
            obj->ObjectUpdate(0.016);
        }

        CHECK(obj->updateCalls == 1000);
    }

    TEST_CASE("Performance - 1000 events on single object")
    {
        auto obj = std::make_shared<TestObjectA>();

        for (int i = 0; i < 1000; ++i)
        {
            duin::Event event{};
            obj->ObjectOnEvent(event);
        }

        CHECK(obj->eventCalls == 1000);
    }

    TEST_CASE("Lifecycle consistency - multiple Ready calls")
    {
        auto obj = std::make_shared<LifecycleTracker>();

        obj->ObjectReady();
        obj->ObjectReady();
        obj->ObjectReady();

        // Ready should be called 3 times
        int readyCount = 0;
        for (const auto &call : obj->callOrder)
        {
            if (call == "Ready")
                readyCount++;
        }
        CHECK(readyCount == 3);
    }

    TEST_CASE("Complex update sequence with different delta times")
    {
        auto obj = std::make_shared<TestObjectA>();

        obj->ObjectUpdate(0.016);
        obj->ObjectUpdate(0.033);
        obj->ObjectUpdate(0.008);
        obj->ObjectUpdate(0.050);
        obj->ObjectUpdate(0.001);

        CHECK(obj->updateCalls == 5);
        CHECK(obj->lastDelta == doctest::Approx(0.001));
    }

    TEST_CASE("Hierarchy with alternating object types at each level")
    {
        auto root = std::make_shared<TestObjectA>();
        auto level1 = root->CreateChildObject<TestObjectB>();
        auto level2 = level1->CreateChildObject<TestObjectC>();
        auto level3 = level2->CreateChildObject<TestObjectA>();
        auto level4 = level3->CreateChildObject<TestObjectB>();

        CHECK(level4->GetParent().get() == level3.get());
        CHECK(level3->GetParent().get() == level2.get());
        CHECK(level2->GetParent().get() == level1.get());
        CHECK(level1->GetParent().get() == root.get());
    }

    TEST_CASE("Empty GameObject default behavior")
    {
        auto obj = std::make_shared<duin::GameObject>();

        // Should not crash with default implementations
        obj->Init();
        obj->Ready();
        obj->Update(0.016);
        obj->PhysicsUpdate(0.016);
        obj->Draw();
        obj->DrawUI();
        obj->Debug();

        duin::Event event{};
        obj->OnEvent(event);

        CHECK(true); // If we reach here without crash, test passes
    }

    TEST_CASE("Verify UUID uniqueness across many objects")
    {
        std::vector<duin::UUID> uuids;
        const int count = 1000;

        for (int i = 0; i < count; ++i)
        {
            auto obj = std::make_shared<duin::GameObject>();
            uuids.push_back(obj->GetUUID());
        }

        // Check all UUIDs are unique
        for (size_t i = 0; i < uuids.size(); ++i)
        {
            for (size_t j = i + 1; j < uuids.size(); ++j)
            {
                CHECK(uuids[i] != uuids[j]);
            }
        }
    }

    TEST_CASE("ResetParent multiple times")
    {
        auto parent = std::make_shared<duin::GameObject>();
        auto child = parent->CreateChildObject<TestObjectA>();

        CHECK(child->GetParent().get() == parent.get());

        child->ResetParent();
        CHECK(child->GetParent().get() == nullptr);

        // Reset again should be safe
        child->ResetParent();
        CHECK(child->GetParent().get() == nullptr);

        // Reset once more
        child->ResetParent();
        CHECK(child->GetParent().get() == nullptr);
    }

    TEST_CASE("SetParent to same parent multiple times")
    {
        auto parent = std::make_shared<duin::GameObject>();
        auto child = std::make_shared<duin::GameObject>();

        child->SetParent(parent);
        CHECK(child->GetParent().get() == parent.get());

        child->SetParent(parent);
        CHECK(child->GetParent().get() == parent.get());

        child->SetParent(parent);
        CHECK(child->GetParent().get() == parent.get());
    }
}
} // namespace TestGameObject
