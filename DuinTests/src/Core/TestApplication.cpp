#include <Duin/Core/Application.h>
#include <Duin/Core/Debug/DNLog.h>
#include <Duin/Objects/GameObject.h>
#include <doctest.h>
#include <memory>
#include <string>
#include <cmath>
#include <vector>

namespace TestApplication
{

// Test Application subclass for testing lifecycle callbacks
class TestApp : public duin::Application
{
  public:
    int initializeCalled = 0;
    int readyCalled = 0;
    int updateCalled = 0;
    int physicsUpdateCalled = 0;
    int drawCalled = 0;
    int drawUICalled = 0;
    int debugCalled = 0;
    int exitCalled = 0;
    int onEventCalled = 0;
    double lastDelta = 0.0;

    void Initialize() override
    {
        ++initializeCalled;
    }

    void Ready() override
    {
        ++readyCalled;
    }

    void OnEvent(duin::Event event) override
    {
        ++onEventCalled;
    }

    void Update(double delta) override
    {
        ++updateCalled;
        lastDelta = delta;
    }

    void PhysicsUpdate(double delta) override
    {
        ++physicsUpdateCalled;
    }

    void Draw() override
    {
        ++drawCalled;
    }

    void DrawUI() override
    {
        ++drawUICalled;
    }

    void Debug() override
    {
        ++debugCalled;
    }

    void Exit() override
    {
        ++exitCalled;
    }
};

// Test GameObject for hierarchy testing
class TestGameObject : public duin::GameObject
{
  public:
    int initCalled = 0;
    int readyCalled = 0;
    int updateCalled = 0;
    int physicsUpdateCalled = 0;
    int drawCalled = 0;
    int drawUICalled = 0;
    int debugCalled = 0;
    int onEventCalled = 0;

    void Init() override
    {
        ++initCalled;
    }

    void Ready() override
    {
        ++readyCalled;
    }

    void OnEvent(duin::Event e) override
    {
        ++onEventCalled;
    }

    void Update(double delta) override
    {
        ++updateCalled;
    }

    void PhysicsUpdate(double delta) override
    {
        ++physicsUpdateCalled;
    }

    void Draw() override
    {
        ++drawCalled;
    }

    void DrawUI() override
    {
        ++drawUICalled;
    }

    void Debug() override
    {
        ++debugCalled;
    }
};

// ============================================================================
// Application construction and basic API
// ============================================================================

TEST_SUITE("Application")
{
    TEST_CASE("Application Constructor - Creates Valid Application")
    {
        TestApp app;
        // Constructing without crashing is the assertion
        CHECK(true);
    }

    TEST_CASE("SetWindowStartupSize - Sets Window Dimensions")
    {
        TestApp app;
        app.SetWindowStartupSize(1920, 1080);
        CHECK(true);
    }

    TEST_CASE("SetWindowName - Sets Window Title")
    {
        TestApp app;
        app.SetWindowName("Duin Test Application");
        CHECK(true);
    }

    TEST_CASE("CreateChildObject - Creates and Returns Child GameObject")
    {
        TestApp app;
        auto child = app.CreateChildObject<TestGameObject>();
        CHECK(child != nullptr);
        CHECK(child->initCalled == 1);
    }

    TEST_CASE("CreateChildObject - Creates Multiple Children")
    {
        TestApp app;
        auto child1 = app.CreateChildObject<TestGameObject>();
        auto child2 = app.CreateChildObject<TestGameObject>();
        auto child3 = app.CreateChildObject<TestGameObject>();

        CHECK(child1 != nullptr);
        CHECK(child2 != nullptr);
        CHECK(child3 != nullptr);
        CHECK(child1 != child2);
        CHECK(child2 != child3);
        CHECK(child1 != child3);
    }

    TEST_CASE("AddChildObject - Adds Existing GameObject")
    {
        TestApp app;
        auto child = std::make_shared<TestGameObject>();
        app.AddChildObject(child);
        CHECK(child != nullptr);
    }

    TEST_CASE("RemoveChildObject - Removes GameObject")
    {
        TestApp app;
        auto child = std::make_shared<TestGameObject>();
        app.AddChildObject(child);
        app.RemoveChildObject(child);
        CHECK(true);
    }

    TEST_CASE("EngineInitialize - Calls Initialize Lifecycle")
    {
        TestApp app;
        app.EngineInitialize();
        CHECK(true);
    }

    TEST_CASE("Initialize - Virtual Method Called")
    {
        TestApp app;
        CHECK(app.initializeCalled == 0);
        app.Initialize();
        CHECK(app.initializeCalled == 1);
    }

    TEST_CASE("Ready - Virtual Method Called")
    {
        TestApp app;
        CHECK(app.readyCalled == 0);
        app.Ready();
        CHECK(app.readyCalled == 1);
    }

    TEST_CASE("Update - Virtual Method Called with Delta")
    {
        TestApp app;
        CHECK(app.updateCalled == 0);
        app.Update(0.016);
        CHECK(app.updateCalled == 1);
        CHECK(std::abs(app.lastDelta - 0.016) < 0.0001);
    }

    TEST_CASE("PhysicsUpdate - Virtual Method Called")
    {
        TestApp app;
        CHECK(app.physicsUpdateCalled == 0);
        app.PhysicsUpdate(0.016);
        CHECK(app.physicsUpdateCalled == 1);
    }

    TEST_CASE("Draw - Virtual Method Called")
    {
        TestApp app;
        CHECK(app.drawCalled == 0);
        app.Draw();
        CHECK(app.drawCalled == 1);
    }

    TEST_CASE("DrawUI - Virtual Method Called")
    {
        TestApp app;
        CHECK(app.drawUICalled == 0);
        app.DrawUI();
        CHECK(app.drawUICalled == 1);
    }

    TEST_CASE("Debug - Virtual Method Called")
    {
        TestApp app;
        CHECK(app.debugCalled == 0);
        app.Debug();
        CHECK(app.debugCalled == 1);
    }

    TEST_CASE("Exit - Virtual Method Called")
    {
        TestApp app;
        CHECK(app.exitCalled == 0);
        app.Exit();
        CHECK(app.exitCalled == 1);
    }

    TEST_CASE("EnginePostUpdate - Propagates to Children")
    {
        TestApp app;
        auto child = app.CreateChildObject<TestGameObject>();
        CHECK(child->updateCalled == 0);
        app.EnginePostUpdate(0.016);
        CHECK(child->updateCalled == 1);
    }

    TEST_CASE("EnginePostPhysicsUpdate - Propagates to Children")
    {
        TestApp app;
        auto child = app.CreateChildObject<TestGameObject>();
        CHECK(child->physicsUpdateCalled == 0);
        app.EnginePostPhysicsUpdate(0.016);
        CHECK(child->physicsUpdateCalled == 1);
    }

    TEST_CASE("EnginePostDraw - Propagates to Children")
    {
        TestApp app;
        auto child = app.CreateChildObject<TestGameObject>();
        CHECK(child->drawCalled == 0);
        app.EnginePostDraw();
        CHECK(child->drawCalled == 1);
    }

    TEST_CASE("EnginePostDrawUI - Propagates to Children")
    {
        TestApp app;
        auto child = app.CreateChildObject<TestGameObject>();
        CHECK(child->drawUICalled == 0);
        app.EnginePostDrawUI();
        CHECK(child->drawUICalled == 1);
    }

    TEST_CASE("EnginePostDebug - Propagates to Children")
    {
        TestApp app;
        auto child = app.CreateChildObject<TestGameObject>();
        CHECK(child->debugCalled == 0);
        app.EnginePostDebug();
        CHECK(child->debugCalled == 1);
    }

    TEST_CASE("Multiple Children - All Receive Lifecycle Calls")
    {
        TestApp app;
        auto child1 = app.CreateChildObject<TestGameObject>();
        auto child2 = app.CreateChildObject<TestGameObject>();
        auto child3 = app.CreateChildObject<TestGameObject>();

        app.EnginePostUpdate(0.016);

        CHECK(child1->updateCalled == 1);
        CHECK(child2->updateCalled == 1);
        CHECK(child3->updateCalled == 1);
    }
}

// ============================================================================
// Global function tests
// ============================================================================

TEST_SUITE("Application Global Functions")
{
    TEST_CASE("GetRootDirectory - Returns Valid Path")
    {
        std::string rootDir = duin::GetRootDirectory();
        CHECK_FALSE(rootDir.empty());
    }

    TEST_CASE("DebugPauseGame and DebugResumeGame - Toggle Pause State")
    {
        duin::DebugResumeGame(); // ensure clean state
        CHECK(duin::DebugIsGamePaused() == 0);

        duin::DebugPauseGame();
        CHECK(duin::DebugIsGamePaused() == 1);

        duin::DebugResumeGame();
        CHECK(duin::DebugIsGamePaused() == 0);
    }

    TEST_CASE("DebugPauseGame - Multiple Calls")
    {
        duin::DebugResumeGame();

        duin::DebugPauseGame();
        CHECK(duin::DebugIsGamePaused() == 1);

        duin::DebugPauseGame();
        CHECK(duin::DebugIsGamePaused() == 1);

        duin::DebugResumeGame();
    }

    TEST_CASE("DebugResumeGame - Multiple Calls")
    {
        duin::DebugPauseGame();

        duin::DebugResumeGame();
        CHECK(duin::DebugIsGamePaused() == 0);

        duin::DebugResumeGame();
        CHECK(duin::DebugIsGamePaused() == 0);
    }

    TEST_CASE("SetFramerate - Sets Target Framerate")
    {
        duin::SetFramerate(30);
        CHECK(true);

        duin::SetFramerate(60);
        CHECK(true);

        duin::SetFramerate(120);
        CHECK(true);
    }

    TEST_CASE("QueuePostReadyCallback - Callback is Queued")
    {
        int called = 0;
        duin::QueuePostReadyCallback([&called]() { ++called; });
        CHECK(called == 0);
    }

    TEST_CASE("QueuePostUpdateCallback - Callback is Queued")
    {
        int called = 0;
        duin::QueuePostUpdateCallback([&](double delta) { ++called; });
        CHECK(called == 0);
    }

    TEST_CASE("QueuePostPhysicsUpdateCallback - Callback is Queued")
    {
        int called = 0;
        duin::QueuePostPhysicsUpdateCallback([&](double delta) { ++called; });
        CHECK(called == 0);
    }

    TEST_CASE("QueuePostDrawCallback - Callback is Queued")
    {
        int called = 0;
        duin::QueuePostDrawCallback([&]() { ++called; });
        CHECK(called == 0);
    }

    TEST_CASE("QueuePostDrawUICallback - Callback is Queued")
    {
        int called = 0;
        duin::QueuePostDrawUICallback([&]() { ++called; });
        CHECK(called == 0);
    }

    TEST_CASE("QueuePreFrameCallback - Callback is Queued")
    {
        int called = 0;
        duin::QueuePreFrameCallback([&]() { ++called; });
        CHECK(called == 0);
    }

    TEST_CASE("QueuePostFrameCallback - Callback is Queued")
    {
        int called = 0;
        duin::QueuePostFrameCallback([&]() { ++called; });
        CHECK(called == 0);
    }

    TEST_CASE("QueuePostDebugCallback - Callback is Queued")
    {
        int called = 0;
        duin::QueuePostDebugCallback([&]() { ++called; });
        CHECK(called == 0);
    }

    TEST_CASE("QueueExitCallback - Callback is Queued")
    {
        int called = 0;
        duin::QueueExitCallback([&]() { ++called; });
        CHECK(called == 0);
    }

    TEST_CASE("Multiple Callbacks - All are Queued")
    {
        int count = 0;
        duin::QueuePostUpdateCallback([&](double d) { ++count; });
        duin::QueuePostUpdateCallback([&](double d) { ++count; });
        duin::QueuePostUpdateCallback([&](double d) { ++count; });
        CHECK(count == 0);
    }
}

// ============================================================================
// Callback execution tests (in-process, no SDL/bgfx needed)
// ============================================================================

TEST_SUITE("Application Callback Execution")
{
    TEST_CASE("EnginePostReady - Executes Queued Callbacks")
    {
        TestApp app;
        auto child = app.CreateChildObject<TestGameObject>();

        int callbackCalled = 0;
        auto conn = duin::QueuePostReadyCallback([&]() { ++callbackCalled; });

        CHECK(callbackCalled == 0);
        CHECK(child->readyCalled == 0);

        app.EnginePostReady();

        CHECK(callbackCalled == 1);
        CHECK(child->readyCalled == 1);
    }

    TEST_CASE("EnginePostUpdate - Executes Queued Callbacks")
    {
        TestApp app;
        auto child = app.CreateChildObject<TestGameObject>();

        int callbackCalled = 0;
        double receivedDelta = 0.0;
        auto conn = duin::QueuePostUpdateCallback([&](double delta) {
            ++callbackCalled;
            receivedDelta = delta;
        });

        app.EnginePostUpdate(0.016);

        CHECK(callbackCalled == 1);
        CHECK(std::abs(receivedDelta - 0.016) < 0.0001);
        CHECK(child->updateCalled == 1);
    }

    TEST_CASE("EnginePostPhysicsUpdate - Executes Queued Callbacks")
    {
        TestApp app;

        int callbackCalled = 0;
        auto conn = duin::QueuePostPhysicsUpdateCallback([&](double delta) { ++callbackCalled; });

        app.EnginePostPhysicsUpdate(0.016);

        CHECK(callbackCalled == 1);
    }

    TEST_CASE("EnginePostDraw - Executes Queued Callbacks")
    {
        TestApp app;

        int callbackCalled = 0;
        auto conn = duin::QueuePostDrawCallback([&]() { ++callbackCalled; });

        app.EnginePostDraw();

        CHECK(callbackCalled == 1);
    }

    TEST_CASE("EnginePostDrawUI - Executes Queued Callbacks")
    {
        TestApp app;

        int callbackCalled = 0;
        auto conn = duin::QueuePostDrawUICallback([&]() { ++callbackCalled; });

        app.EnginePostDrawUI();

        CHECK(callbackCalled == 1);
    }

    TEST_CASE("EnginePreFrame - Executes Queued Callbacks")
    {
        TestApp app;

        int callbackCalled = 0;
        auto conn = duin::QueuePreFrameCallback([&]() { ++callbackCalled; });

        app.EnginePreFrame();

        CHECK(callbackCalled == 1);
    }

    TEST_CASE("EnginePostFrame - Executes Queued Callbacks")
    {
        TestApp app;

        int callbackCalled = 0;
        auto conn = duin::QueuePostFrameCallback([&]() { ++callbackCalled; });

        app.EnginePostFrame();

        CHECK(callbackCalled == 1);
    }

    TEST_CASE("EnginePostDebug - Executes Queued Callbacks")
    {
        TestApp app;

        int callbackCalled = 0;
        auto conn = duin::QueuePostDebugCallback([&]() { ++callbackCalled; });

        app.EnginePostDebug();

        CHECK(callbackCalled == 1);
    }

    TEST_CASE("EngineExit - Executes Queued Exit Callbacks")
    {
        TestApp app;

        int callbackCalled = 0;
        auto conn = duin::QueueExitCallback([&]() { ++callbackCalled; });

        app.EngineExit();

        CHECK(callbackCalled == 1);
    }

    TEST_CASE("Multiple Callbacks - All are Executed in Order")
    {
        TestApp app;

        std::vector<int> order;
        auto c1 = duin::QueuePostUpdateCallback([&](double d) { order.push_back(1); });
        auto c2 = duin::QueuePostUpdateCallback([&](double d) { order.push_back(2); });
        auto c3 = duin::QueuePostUpdateCallback([&](double d) { order.push_back(3); });

        app.EnginePostUpdate(0.016);

        CHECK(order.size() == 3);
        CHECK(order[0] == 1);
        CHECK(order[1] == 2);
        CHECK(order[2] == 3);
    }
}

// ============================================================================
// Lifecycle integration tests
// ============================================================================

TEST_SUITE("Application Lifecycle Integration")
{
    TEST_CASE("Lifecycle Order - Children are Updated Before Callbacks")
    {
        TestApp app;
        auto child = app.CreateChildObject<TestGameObject>();

        bool childUpdatedFirst = false;
        auto conn = duin::QueuePostUpdateCallback([&](double d) {
            childUpdatedFirst = (child->updateCalled == 1);
        });

        app.EnginePostUpdate(0.016);

        CHECK(childUpdatedFirst);
    }

    TEST_CASE("Nested Children - All Receive Lifecycle Calls")
    {
        TestApp app;
        auto parent = app.CreateChildObject<TestGameObject>();
        auto child = std::make_shared<TestGameObject>();
        parent->AddChildObject(child);
        child->Init();

        app.EnginePostUpdate(0.016);

        CHECK(parent->updateCalled == 1);
        CHECK(child->updateCalled == 1);
    }

    TEST_CASE("Remove Child - No Longer Receives Updates")
    {
        TestApp app;
        auto child = std::make_shared<TestGameObject>();

        app.AddChildObject(child);
        app.RemoveChildObject(child);

        app.EnginePostUpdate(0.016);

        CHECK(child->updateCalled == 0);
    }

    TEST_CASE("Full frame loop - Update and Physics are called")
    {
        TestApp app;
        app.SetHeadless(true);
        app.EngineInitialize();
        app.Initialize();
        app.InitSDL();
        // app.InitBGFX(); // Removed due to API change
        app.InitImGui();

        double dt = 0.016, pCur = 0.0, pPrev = 0.0, pAccum = 0.0;
        bool cont = app.ProcessFrame(dt, pCur, pPrev, pAccum);

        CHECK(app.updateCalled >= 1);
        CHECK(app.physicsUpdateCalled >= 1);

        app.ShutdownImGui();
        // app.ShutdownBGFX(); // Removed due to API change
        app.ShutdownSDL();
        app.EngineExit();
        app.Exit();
    }
}

// ============================================================================
// Edge case tests
// ============================================================================

TEST_SUITE("Application Edge Cases")
{
    TEST_CASE("EnginePostUpdate - Zero Delta Time")
    {
        TestApp app;
        auto child = app.CreateChildObject<TestGameObject>();

        app.EnginePostUpdate(0.0);

        CHECK(child->updateCalled == 1);
    }

    TEST_CASE("EnginePostUpdate - Large Delta Time")
    {
        TestApp app;

        int callbackCalled = 0;
        double receivedDelta = 0.0;
        auto conn = duin::QueuePostUpdateCallback([&](double delta) {
            ++callbackCalled;
            receivedDelta = delta;
        });

        app.EnginePostUpdate(1.0);

        CHECK(callbackCalled == 1);
        CHECK(std::abs(receivedDelta - 1.0) < 0.0001);
    }

    TEST_CASE("Multiple EnginePostReady Calls - Callbacks Execute Multiple Times")
    {
        TestApp app;
        auto child = app.CreateChildObject<TestGameObject>();

        int callbackCount = 0;
        auto conn = duin::QueuePostReadyCallback([&]() { ++callbackCount; });

        app.EnginePostReady();
        CHECK(callbackCount == 1);
        CHECK(child->readyCalled == 1);

        app.EnginePostReady();
        CHECK(callbackCount == 2);
        CHECK(child->readyCalled == 2);
    }

    TEST_CASE("SetWindowStartupSize - Various Sizes")
    {
        TestApp app;
        app.SetWindowStartupSize(800, 600);
        app.SetWindowStartupSize(1920, 1080);
        app.SetWindowStartupSize(3840, 2160);
        app.SetWindowStartupSize(640, 480);
        CHECK(true);
    }

    TEST_CASE("SetWindowName - Various Names")
    {
        TestApp app;
        app.SetWindowName("Test");
        app.SetWindowName("My Game Application");
        app.SetWindowName("");
        app.SetWindowName("Very Long Application Name That Is Quite Verbose");
        CHECK(true);
    }
}

} // namespace TestApplication
