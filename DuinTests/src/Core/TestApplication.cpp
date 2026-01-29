#include <Duin/Core/Application.h>
#include <Duin/Core/Debug/DNLog.h>
#include <Duin/Objects/GameObject.h>
#include <doctest.h>
#include <memory>
#include <string>
#include <SDL3/SDL.h>
#include <SDL3/SDL_process.h>
#include <cstring>
#include <vector>
#include <cmath>

#ifdef _WIN32
#include <windows.h>
#endif

#if 0
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
// Process-based test isolation helpers
// ============================================================================

// Helper function to run a test in a separate process
// Returns true if test passed, false if failed
bool RunTestInProcess(const char* testName)
{
    // Get the current executable path from argv[0] or use SDL
    static std::string executablePath;

    if (executablePath.empty())
    {
        // On Windows, we can get the executable path directly
#ifdef _WIN32
        char buffer[MAX_PATH];
        GetModuleFileNameA(NULL, buffer, MAX_PATH);
        executablePath = buffer;
#else
        // On other platforms, try SDL_GetBasePath
        char* basePath = SDL_GetBasePath();
        if (basePath)
        {
            executablePath = std::string(basePath) + "DuinTests";
            SDL_free(basePath);
        }
        else
        {
            executablePath = "./DuinTests";
        }
#endif
    }

    // Setup arguments: executable path, --run-test-isolated, test name, NULL terminator
    const char* args[] = {
        executablePath.c_str(),
        "--run-test-isolated",
        testName,
        nullptr
    };

    // Create the process without stdio piping (tests will use their own output)
    SDL_Process* process = SDL_CreateProcess(args, false);

    if (!process)
    {
        fprintf(stderr, "Failed to create process for test '%s': %s\n", testName, SDL_GetError());
        fprintf(stderr, "Executable path used: %s\n", executablePath.c_str());
        return false;
    }

    // Wait for the process to complete
    int exitCode = 0;
    if (!SDL_WaitProcess(process, true, &exitCode))
    {
        fprintf(stderr, "Failed to wait for process for test '%s': %s\n", testName, SDL_GetError());
        SDL_DestroyProcess(process);
        return false;
    }

    SDL_DestroyProcess(process);

    // Exit code 0 means test passed
    return exitCode == 0;
}

// Individual test execution function - called when --run-test-isolated is passed
// Returns 0 on success, 1 on failure
int RunIsolatedTest(const char* testName)
{
    // Map test names to individual test execution
    // Each test creates its own Application instance in isolation

    if (strcmp(testName, "Application Constructor - Creates Valid Application") == 0)
    {
        TestApp app;
        return 0; // Success
    }
    else if (strcmp(testName, "SetWindowStartupSize - Sets Window Dimensions") == 0)
    {
        TestApp app;
        app.SetWindowStartupSize(1920, 1080);
        return 0;
    }
    else if (strcmp(testName, "SetWindowName - Sets Window Title") == 0)
    {
        TestApp app;
        app.SetWindowName("Duin Test Application");
        return 0;
    }
    else if (strcmp(testName, "CreateChildObject - Creates and Returns Child GameObject") == 0)
    {
        TestApp app;
        auto child = app.CreateChildObject<TestGameObject>();
        if (child == nullptr) return 1;
        if (child->initCalled != 1) return 1;
        return 0;
    }
    else if (strcmp(testName, "CreateChildObject - Creates Multiple Children") == 0)
    {
        TestApp app;
        auto child1 = app.CreateChildObject<TestGameObject>();
        auto child2 = app.CreateChildObject<TestGameObject>();
        auto child3 = app.CreateChildObject<TestGameObject>();

        if (child1 == nullptr || child2 == nullptr || child3 == nullptr) return 1;
        if (child1 == child2 || child2 == child3 || child1 == child3) return 1;
        return 0;
    }
    else if (strcmp(testName, "AddChildObject - Adds Existing GameObject") == 0)
    {
        TestApp app;
        auto child = std::make_shared<TestGameObject>();
        app.AddChildObject(child);
        if (child == nullptr) return 1;
        return 0;
    }
    else if (strcmp(testName, "RemoveChildObject - Removes GameObject") == 0)
    {
        TestApp app;
        auto child = std::make_shared<TestGameObject>();
        app.AddChildObject(child);
        app.RemoveChildObject(child);
        return 0;
    }
    else if (strcmp(testName, "EngineInitialize - Calls Initialize Lifecycle") == 0)
    {
        TestApp app;
        app.EngineInitialize();
        return 0;
    }
    else if (strcmp(testName, "Initialize - Virtual Method Called") == 0)
    {
        TestApp app;
        if (app.initializeCalled != 0) return 1;
        app.Initialize();
        if (app.initializeCalled != 1) return 1;
        return 0;
    }
    else if (strcmp(testName, "Ready - Virtual Method Called") == 0)
    {
        TestApp app;
        if (app.readyCalled != 0) return 1;
        app.Ready();
        if (app.readyCalled != 1) return 1;
        return 0;
    }
    else if (strcmp(testName, "Update - Virtual Method Called with Delta") == 0)
    {
        TestApp app;
        if (app.updateCalled != 0) return 1;
        app.Update(0.016);
        if (app.updateCalled != 1) return 1;
        if (std::abs(app.lastDelta - 0.016) > 0.0001) return 1;
        return 0;
    }
    else if (strcmp(testName, "PhysicsUpdate - Virtual Method Called") == 0)
    {
        TestApp app;
        if (app.physicsUpdateCalled != 0) return 1;
        app.PhysicsUpdate(0.016);
        if (app.physicsUpdateCalled != 1) return 1;
        return 0;
    }
    else if (strcmp(testName, "Draw - Virtual Method Called") == 0)
    {
        TestApp app;
        if (app.drawCalled != 0) return 1;
        app.Draw();
        if (app.drawCalled != 1) return 1;
        return 0;
    }
    else if (strcmp(testName, "DrawUI - Virtual Method Called") == 0)
    {
        TestApp app;
        if (app.drawUICalled != 0) return 1;
        app.DrawUI();
        if (app.drawUICalled != 1) return 1;
        return 0;
    }
    else if (strcmp(testName, "Debug - Virtual Method Called") == 0)
    {
        TestApp app;
        if (app.debugCalled != 0) return 1;
        app.Debug();
        if (app.debugCalled != 1) return 1;
        return 0;
    }
    else if (strcmp(testName, "Exit - Virtual Method Called") == 0)
    {
        TestApp app;
        if (app.exitCalled != 0) return 1;
        app.Exit();
        if (app.exitCalled != 1) return 1;
        return 0;
    }
    else if (strcmp(testName, "EnginePostUpdate - Propagates to Children") == 0)
    {
        TestApp app;
        auto child = app.CreateChildObject<TestGameObject>();
        if (child->updateCalled != 0) return 1;
        app.EnginePostUpdate(0.016);
        if (child->updateCalled != 1) return 1;
        return 0;
    }
    else if (strcmp(testName, "EnginePostPhysicsUpdate - Propagates to Children") == 0)
    {
        TestApp app;
        auto child = app.CreateChildObject<TestGameObject>();
        if (child->physicsUpdateCalled != 0) return 1;
        app.EnginePostPhysicsUpdate(0.016);
        if (child->physicsUpdateCalled != 1) return 1;
        return 0;
    }
    else if (strcmp(testName, "EnginePostDraw - Propagates to Children") == 0)
    {
        TestApp app;
        auto child = app.CreateChildObject<TestGameObject>();
        if (child->drawCalled != 0) return 1;
        app.EnginePostDraw();
        if (child->drawCalled != 1) return 1;
        return 0;
    }
    else if (strcmp(testName, "EnginePostDrawUI - Propagates to Children") == 0)
    {
        TestApp app;
        auto child = app.CreateChildObject<TestGameObject>();
        if (child->drawUICalled != 0) return 1;
        app.EnginePostDrawUI();
        if (child->drawUICalled != 1) return 1;
        return 0;
    }
    else if (strcmp(testName, "EnginePostDebug - Propagates to Children") == 0)
    {
        TestApp app;
        auto child = app.CreateChildObject<TestGameObject>();
        if (child->debugCalled != 0) return 1;
        app.EnginePostDebug();
        if (child->debugCalled != 1) return 1;
        return 0;
    }
    else if (strcmp(testName, "Multiple Children - All Receive Lifecycle Calls") == 0)
    {
        TestApp app;
        auto child1 = app.CreateChildObject<TestGameObject>();
        auto child2 = app.CreateChildObject<TestGameObject>();
        auto child3 = app.CreateChildObject<TestGameObject>();

        app.EnginePostUpdate(0.016);

        if (child1->updateCalled != 1) return 1;
        if (child2->updateCalled != 1) return 1;
        if (child3->updateCalled != 1) return 1;
        return 0;
    }
    // Application Callback Execution tests
    else if (strcmp(testName, "EnginePostReady - Executes Queued Callbacks") == 0)
    {
        TestApp app;
        auto child = app.CreateChildObject<TestGameObject>();

        int callbackCalled = 0;
        duin::QueuePostReadyCallback([&]() { ++callbackCalled; });

        if (callbackCalled != 0) return 1;
        if (child->readyCalled != 0) return 1;

        app.EnginePostReady();

        if (callbackCalled != 1) return 1;
        if (child->readyCalled != 1) return 1;
        return 0;
    }
    else if (strcmp(testName, "EnginePostUpdate - Executes Queued Callbacks") == 0)
    {
        TestApp app;
        auto child = app.CreateChildObject<TestGameObject>();

        int callbackCalled = 0;
        double receivedDelta = 0.0;

        duin::QueuePostUpdateCallback([&](double delta) {
            ++callbackCalled;
            receivedDelta = delta;
        });

        app.EnginePostUpdate(0.016);

        if (callbackCalled != 1) return 1;
        if (std::abs(receivedDelta - 0.016) > 0.0001) return 1;
        if (child->updateCalled != 1) return 1;
        return 0;
    }
    else if (strcmp(testName, "EnginePostPhysicsUpdate - Executes Queued Callbacks") == 0)
    {
        TestApp app;

        int callbackCalled = 0;

        duin::QueuePostPhysicsUpdateCallback([&](double delta) { ++callbackCalled; });

        app.EnginePostPhysicsUpdate(0.016);

        if (callbackCalled != 1) return 1;
        return 0;
    }
    else if (strcmp(testName, "EnginePostDraw - Executes Queued Callbacks") == 0)
    {
        TestApp app;

        int callbackCalled = 0;

        duin::QueuePostDrawCallback([&]() { ++callbackCalled; });

        app.EnginePostDraw();

        if (callbackCalled != 1) return 1;
        return 0;
    }
    else if (strcmp(testName, "EnginePostDrawUI - Executes Queued Callbacks") == 0)
    {
        TestApp app;

        int callbackCalled = 0;

        duin::QueuePostDrawUICallback([&]() { ++callbackCalled; });

        app.EnginePostDrawUI();

        if (callbackCalled != 1) return 1;
        return 0;
    }
    else if (strcmp(testName, "EnginePreFrame - Executes Queued Callbacks") == 0)
    {
        TestApp app;

        int callbackCalled = 0;

        duin::QueuePreFrameCallback([&]() { ++callbackCalled; });

        app.EnginePreFrame();

        if (callbackCalled != 1) return 1;
        return 0;
    }
    else if (strcmp(testName, "EnginePostFrame - Executes Queued Callbacks") == 0)
    {
        TestApp app;

        int callbackCalled = 0;

        duin::QueuePostFrameCallback([&]() { ++callbackCalled; });

        app.EnginePostFrame();

        if (callbackCalled != 1) return 1;
        return 0;
    }
    else if (strcmp(testName, "EnginePostDebug - Executes Queued Callbacks") == 0)
    {
        TestApp app;

        int callbackCalled = 0;

        duin::QueuePostDebugCallback([&]() { ++callbackCalled; });

        app.EnginePostDebug();

        if (callbackCalled != 1) return 1;
        return 0;
    }
    else if (strcmp(testName, "EngineExit - Executes Queued Exit Callbacks") == 0)
    {
        TestApp app;

        int callbackCalled = 0;

        duin::QueueExitCallback([&]() { ++callbackCalled; });

        app.EngineExit();

        if (callbackCalled != 1) return 1;
        return 0;
    }
    else if (strcmp(testName, "Multiple Callbacks - All are Executed in Order") == 0)
    {
        TestApp app;

        std::vector<int> order;

        duin::QueuePostUpdateCallback([&](double d) { order.push_back(1); });
        duin::QueuePostUpdateCallback([&](double d) { order.push_back(2); });
        duin::QueuePostUpdateCallback([&](double d) { order.push_back(3); });

        app.EnginePostUpdate(0.016);

        if (order.size() != 3) return 1;
        if (order[0] != 1) return 1;
        if (order[1] != 2) return 1;
        if (order[2] != 3) return 1;
        return 0;
    }
    // Application Lifecycle Integration tests
    else if (strcmp(testName, "Lifecycle Order - Children are Updated Before Callbacks") == 0)
    {
        TestApp app;
        auto child = app.CreateChildObject<TestGameObject>();

        bool childUpdatedFirst = false;

        duin::QueuePostUpdateCallback([&](double d) {
            // If child was updated, it should have updateCalled == 1
            childUpdatedFirst = (child->updateCalled == 1);
        });

        app.EnginePostUpdate(0.016);

        if (!childUpdatedFirst) return 1;
        return 0;
    }
    else if (strcmp(testName, "Nested Children - All Receive Lifecycle Calls") == 0)
    {
        TestApp app;
        auto parent = app.CreateChildObject<TestGameObject>();
        auto child = std::make_shared<TestGameObject>();
        parent->AddChildObject(child);
        child->Init(); // Manually init since not using CreateChildObject

        app.EnginePostUpdate(0.016);

        if (parent->updateCalled != 1) return 1;
        if (child->updateCalled != 1) return 1;
        return 0;
    }
    else if (strcmp(testName, "Remove Child - No Longer Receives Updates") == 0)
    {
        TestApp app;
        auto child = std::make_shared<TestGameObject>();

        app.AddChildObject(child);
        app.RemoveChildObject(child);

        app.EnginePostUpdate(0.016);

        // Child was removed, should not be updated
        if (child->updateCalled != 0) return 1;
        return 0;
    }
    // Application Edge Cases tests
    else if (strcmp(testName, "EnginePostUpdate - Zero Delta Time") == 0)
    {
        TestApp app;
        auto child = app.CreateChildObject<TestGameObject>();

        app.EnginePostUpdate(0.0);

        if (child->updateCalled != 1) return 1;
        return 0;
    }
    else if (strcmp(testName, "EnginePostUpdate - Large Delta Time") == 0)
    {
        TestApp app;

        int callbackCalled = 0;
        double receivedDelta = 0.0;

        duin::QueuePostUpdateCallback([&](double delta) {
            ++callbackCalled;
            receivedDelta = delta;
        });

        app.EnginePostUpdate(1.0);

        if (callbackCalled != 1) return 1;
        if (std::abs(receivedDelta - 1.0) > 0.0001) return 1;
        return 0;
    }
    else if (strcmp(testName, "Multiple EnginePostReady Calls - Callbacks Execute Multiple Times") == 0)
    {
        TestApp app;
        auto child = app.CreateChildObject<TestGameObject>();

        int callbackCount = 0;
        duin::QueuePostReadyCallback([&]() { ++callbackCount; });

        app.EnginePostReady();
        if (callbackCount != 1) return 1;
        if (child->readyCalled != 1) return 1;

        // Second call should execute callbacks again
        app.EnginePostReady();
        if (callbackCount != 2) return 1;
        if (child->readyCalled != 2) return 1;
        return 0;
    }
    else if (strcmp(testName, "SetWindowStartupSize - Various Sizes") == 0)
    {
        TestApp app;

        app.SetWindowStartupSize(800, 600);
        app.SetWindowStartupSize(1920, 1080);
        app.SetWindowStartupSize(3840, 2160);
        app.SetWindowStartupSize(640, 480);

        // Should not crash
        return 0;
    }
    else if (strcmp(testName, "SetWindowName - Various Names") == 0)
    {
        TestApp app;

        app.SetWindowName("Test");
        app.SetWindowName("My Game Application");
        app.SetWindowName("");
        app.SetWindowName("Very Long Application Name That Is Quite Verbose");

        // Should not crash
        return 0;
    }

    fprintf(stderr, "Unknown test name: %s\n", testName);
    return 1; // Unknown test
}

// ============================================================================
// Process-isolated test cases
// ============================================================================

TEST_SUITE("Application")
{
    TEST_CASE("Application Constructor - Creates Valid Application")
    {
        CHECK(RunTestInProcess("Application Constructor - Creates Valid Application"));
    }

    TEST_CASE("SetWindowStartupSize - Sets Window Dimensions")
    {
        CHECK(RunTestInProcess("SetWindowStartupSize - Sets Window Dimensions"));
    }

    TEST_CASE("SetWindowName - Sets Window Title")
    {
        CHECK(RunTestInProcess("SetWindowName - Sets Window Title"));
    }

    TEST_CASE("CreateChildObject - Creates and Returns Child GameObject")
    {
        CHECK(RunTestInProcess("CreateChildObject - Creates and Returns Child GameObject"));
    }

    TEST_CASE("CreateChildObject - Creates Multiple Children")
    {
        CHECK(RunTestInProcess("CreateChildObject - Creates Multiple Children"));
    }

    TEST_CASE("AddChildObject - Adds Existing GameObject")
    {
        CHECK(RunTestInProcess("AddChildObject - Adds Existing GameObject"));
    }

    TEST_CASE("RemoveChildObject - Removes GameObject")
    {
        CHECK(RunTestInProcess("RemoveChildObject - Removes GameObject"));
    }

    TEST_CASE("EngineInitialize - Calls Initialize Lifecycle")
    {
        CHECK(RunTestInProcess("EngineInitialize - Calls Initialize Lifecycle"));
    }

    TEST_CASE("Initialize - Virtual Method Called")
    {
        CHECK(RunTestInProcess("Initialize - Virtual Method Called"));
    }

    TEST_CASE("Ready - Virtual Method Called")
    {
        CHECK(RunTestInProcess("Ready - Virtual Method Called"));
    }

    TEST_CASE("Update - Virtual Method Called with Delta")
    {
        CHECK(RunTestInProcess("Update - Virtual Method Called with Delta"));
    }

    TEST_CASE("PhysicsUpdate - Virtual Method Called")
    {
        CHECK(RunTestInProcess("PhysicsUpdate - Virtual Method Called"));
    }

    TEST_CASE("Draw - Virtual Method Called")
    {
        CHECK(RunTestInProcess("Draw - Virtual Method Called"));
    }

    TEST_CASE("DrawUI - Virtual Method Called")
    {
        CHECK(RunTestInProcess("DrawUI - Virtual Method Called"));
    }

    TEST_CASE("Debug - Virtual Method Called")
    {
        CHECK(RunTestInProcess("Debug - Virtual Method Called"));
    }

    TEST_CASE("Exit - Virtual Method Called")
    {
        CHECK(RunTestInProcess("Exit - Virtual Method Called"));
    }

    TEST_CASE("EnginePostUpdate - Propagates to Children")
    {
        CHECK(RunTestInProcess("EnginePostUpdate - Propagates to Children"));
    }

    TEST_CASE("EnginePostPhysicsUpdate - Propagates to Children")
    {
        CHECK(RunTestInProcess("EnginePostPhysicsUpdate - Propagates to Children"));
    }

    TEST_CASE("EnginePostDraw - Propagates to Children")
    {
        CHECK(RunTestInProcess("EnginePostDraw - Propagates to Children"));
    }

    TEST_CASE("EnginePostDrawUI - Propagates to Children")
    {
        CHECK(RunTestInProcess("EnginePostDrawUI - Propagates to Children"));
    }

    TEST_CASE("EnginePostDebug - Propagates to Children")
    {
        CHECK(RunTestInProcess("EnginePostDebug - Propagates to Children"));
    }

    TEST_CASE("Multiple Children - All Receive Lifecycle Calls")
    {
        CHECK(RunTestInProcess("Multiple Children - All Receive Lifecycle Calls"));
    }
}

TEST_SUITE("Application Global Functions")
{
    TEST_CASE("GetRootDirectory - Returns Valid Path")
    {
        std::string rootDir = duin::GetRootDirectory();

        CHECK_FALSE(rootDir.empty());
    }

    TEST_CASE("DebugPauseGame and DebugResumeGame - Toggle Pause State")
    {
        CHECK(duin::DebugIsGamePaused() == 0);

        duin::DebugPauseGame();
        CHECK(duin::DebugIsGamePaused() == 1);

        duin::DebugResumeGame();
        CHECK(duin::DebugIsGamePaused() == 0);
    }

    TEST_CASE("DebugPauseGame - Multiple Calls")
    {
        duin::DebugResumeGame(); // Reset state

        duin::DebugPauseGame();
        CHECK(duin::DebugIsGamePaused() == 1);

        duin::DebugPauseGame(); // Pause again
        CHECK(duin::DebugIsGamePaused() == 1);
    }

    TEST_CASE("DebugResumeGame - Multiple Calls")
    {
        duin::DebugPauseGame(); // Set to paused

        duin::DebugResumeGame();
        CHECK(duin::DebugIsGamePaused() == 0);

        duin::DebugResumeGame(); // Resume again
        CHECK(duin::DebugIsGamePaused() == 0);
    }

    TEST_CASE("SetFramerate - Sets Target Framerate")
    {
        duin::SetFramerate(30);
        // Cannot directly verify, but should not crash
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

        // Callback is queued but not yet called
        CHECK(called == 0);
    }

    TEST_CASE("QueuePostUpdateCallback - Callback is Queued")
    {
        int called = 0;
        double receivedDelta = 0.0;

        duin::QueuePostUpdateCallback([&](double delta) {
            ++called;
            receivedDelta = delta;
        });

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

        // All queued but not called yet
        CHECK(count == 0);
    }
}

TEST_SUITE("Application Callback Execution")
{
    TEST_CASE("EnginePostReady - Executes Queued Callbacks")
    {
        CHECK(RunTestInProcess("EnginePostReady - Executes Queued Callbacks"));
    }

    TEST_CASE("EnginePostUpdate - Executes Queued Callbacks")
    {
        CHECK(RunTestInProcess("EnginePostUpdate - Executes Queued Callbacks"));
    }

    TEST_CASE("EnginePostPhysicsUpdate - Executes Queued Callbacks")
    {
        CHECK(RunTestInProcess("EnginePostPhysicsUpdate - Executes Queued Callbacks"));
    }

    TEST_CASE("EnginePostDraw - Executes Queued Callbacks")
    {
        CHECK(RunTestInProcess("EnginePostDraw - Executes Queued Callbacks"));
    }

    TEST_CASE("EnginePostDrawUI - Executes Queued Callbacks")
    {
        CHECK(RunTestInProcess("EnginePostDrawUI - Executes Queued Callbacks"));
    }

    TEST_CASE("EnginePreFrame - Executes Queued Callbacks")
    {
        CHECK(RunTestInProcess("EnginePreFrame - Executes Queued Callbacks"));
    }

    TEST_CASE("EnginePostFrame - Executes Queued Callbacks")
    {
        CHECK(RunTestInProcess("EnginePostFrame - Executes Queued Callbacks"));
    }

    TEST_CASE("EnginePostDebug - Executes Queued Callbacks")
    {
        CHECK(RunTestInProcess("EnginePostDebug - Executes Queued Callbacks"));
    }

    TEST_CASE("EngineExit - Executes Queued Exit Callbacks")
    {
        CHECK(RunTestInProcess("EngineExit - Executes Queued Exit Callbacks"));
    }

    TEST_CASE("Multiple Callbacks - All are Executed in Order")
    {
        CHECK(RunTestInProcess("Multiple Callbacks - All are Executed in Order"));
    }
}

TEST_SUITE("Application Lifecycle Integration")
{
    TEST_CASE("Lifecycle Order - Children are Updated Before Callbacks")
    {
        CHECK(RunTestInProcess("Lifecycle Order - Children are Updated Before Callbacks"));
    }

    TEST_CASE("Nested Children - All Receive Lifecycle Calls")
    {
        CHECK(RunTestInProcess("Nested Children - All Receive Lifecycle Calls"));
    }

    TEST_CASE("Remove Child - No Longer Receives Updates")
    {
        CHECK(RunTestInProcess("Remove Child - No Longer Receives Updates"));
    }
}

TEST_SUITE("Application Edge Cases")
{
    TEST_CASE("EnginePostUpdate - Zero Delta Time")
    {
        CHECK(RunTestInProcess("EnginePostUpdate - Zero Delta Time"));
    }

    TEST_CASE("EnginePostUpdate - Large Delta Time")
    {
        CHECK(RunTestInProcess("EnginePostUpdate - Large Delta Time"));
    }

    TEST_CASE("Multiple EnginePostReady Calls - Callbacks Execute Multiple Times")
    {
        CHECK(RunTestInProcess("Multiple EnginePostReady Calls - Callbacks Execute Multiple Times"));
    }

    TEST_CASE("SetWindowStartupSize - Various Sizes")
    {
        CHECK(RunTestInProcess("SetWindowStartupSize - Various Sizes"));
    }

    TEST_CASE("SetWindowName - Various Names")
    {
        CHECK(RunTestInProcess("SetWindowName - Various Names"));
    }
}

#endif
