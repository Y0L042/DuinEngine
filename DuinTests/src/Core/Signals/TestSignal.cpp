#include <doctest.h>
#include <Duin/Core/Signals/Signal.h>
#include <Duin/Core/Debug/DNLog.h>
#include <vector>
#include <string>
#include <cstdint>
#include <map>
#include <memory>

// Example custom struct for complex type testing
struct CustomData
{
    int id;
    std::string name;
    bool operator==(const CustomData &other) const
    {
        return id == other.id && name == other.name;
    }
};

namespace TestSignal
{
TEST_SUITE("Signal")
{
    TEST_CASE("Signal UUID is valid and unique")
    {
        duin::Signal<> sig1;
        duin::Signal<> sig2;
        CHECK(sig1.GetUUID() != duin::UUID::INVALID);
        CHECK(sig2.GetUUID() != duin::UUID::INVALID);
        CHECK(sig1.GetUUID() != sig2.GetUUID());
    }

    TEST_CASE("Connect returns valid UUID and listener is called")
    {
        duin::Signal<int> sig;
        int called = 0;
        duin::UUID id = sig.Connect([&](int v) { called = v; });
        CHECK(id != duin::UUID::INVALID);
        CHECK(sig.GetListenerCount() == 1);

        sig.Emit(42);
        CHECK(called == 42);
    }

    TEST_CASE("Multiple listeners are called")
    {
        duin::Signal<std::string> sig;
        std::vector<std::string> results;
        sig.Connect([&](const std::string &s) { results.push_back("A:" + s); });
        sig.Connect([&](const std::string &s) { results.push_back("B:" + s); });

        sig.Emit("test");
        DN_INFO("{}", results[0]);
        CHECK(sig.GetListenerCount() == 2);
        CHECK(results.size() == 2);
        CHECK(results[0] == "A:test");
        CHECK(results[1] == "B:test");
    }

    TEST_CASE("Disconnect removes listener and returns true")
    {
        duin::Signal<int> sig;
        int called = 0;
        duin::UUID id = sig.Connect([&](int v) { called = v; });

        bool disconnected = sig.Disconnect(id);
        CHECK(disconnected);

        called = 0;
        sig.Emit(99);
        CHECK(called == 0); // Should not be called after disconnect
    }

    TEST_CASE("Disconnect returns false for unknown UUID")
    {
        duin::Signal<> sig;
        duin::UUID fake(123456);
        CHECK_FALSE(sig.Disconnect(fake));
    }

    TEST_CASE("Emit with no listeners does nothing")
    {
        duin::Signal<int> sig;
        // Should not crash or throw
        sig.Emit(123);
        CHECK(true); // If we reach here, test passes
    }

    TEST_CASE("Listener can disconnect itself during emit")
    {
        duin::Signal<int> sig;
        int callCount = 0;
        duin::UUID id = sig.Connect([&](int v) {
            ++callCount;
            sig.Disconnect(id);
        });

        sig.Emit(1);
        sig.Emit(2);
        CHECK(callCount == 1); // Only called once
    }

    TEST_CASE("Listeners with different argument types")
    {
        duin::Signal<int, std::string, double> sig;
        int i = 0;
        std::string s;
        double d = 0.0;
        sig.Connect([&](int a, const std::string &b, double c) {
            i = a;
            s = b;
            d = c;
        });

        sig.Emit(7, "hello", 3.14);
        CHECK(i == 7);
        CHECK(s == "hello");
        CHECK(d == doctest::Approx(3.14));
    }

    TEST_CASE("Multiple connects and disconnects")
    {
        duin::Signal<int> sig;
        int a = 0, b = 0;
        duin::UUID idA = sig.Connect([&](int v) { a = v; });
        duin::UUID idB = sig.Connect([&](int v) { b = v; });

        sig.Emit(5);
        CHECK(a == 5);
        CHECK(b == 5);

        sig.Disconnect(idA);
        a = 0;
        b = 0;
        sig.Emit(10);
        CHECK(a == 0);
        CHECK(b == 10);

        sig.Disconnect(idB);
        a = b = 0;
        sig.Emit(20);
        CHECK(a == 0);
        CHECK(b == 0);
    }

    TEST_CASE("Connect nullptr callback is ignored")
    {
        duin::Signal<int> sig;
        duin::UUID id = sig.Connect(nullptr);
        // Should not crash or call anything
        sig.Emit(1);
        CHECK(sig.Disconnect(id));
    }

    TEST_CASE("Emit and receive std::vector<int>")
    {
        duin::Signal<std::vector<int>> sig;
        std::vector<int> received;
        sig.Connect([&](const std::vector<int> &v) { received = v; });
        std::vector<int> testVec = {1, 2, 3, 4};
        sig.Emit(testVec);
        CHECK(received == testVec);
    }

    TEST_CASE("Emit and receive std::map<std::string, int>")
    {
        duin::Signal<std::map<std::string, int>> sig;
        std::map<std::string, int> received;
        sig.Connect([&](const std::map<std::string, int> &m) { received = m; });
        std::map<std::string, int> testMap = {{"a", 1}, {"b", 2}};
        sig.Emit(testMap);
        CHECK(received == testMap);
    }

    TEST_CASE("Emit and receive custom struct")
    {
        duin::Signal<CustomData> sig;
        CustomData received;
        sig.Connect([&](const CustomData &data) { received = data; });
        CustomData testData{42, "Duin"};
        sig.Emit(testData);
        CHECK(received == testData);
    }

    TEST_CASE("Emit and receive pointer to custom struct")
    {
        duin::Signal<std::shared_ptr<CustomData>> sig;
        std::shared_ptr<CustomData> received;
        sig.Connect([&](std::shared_ptr<CustomData> data) { received = data; });
        auto testPtr = std::make_shared<CustomData>(CustomData{7, "Signal"});
        sig.Emit(testPtr);
        CHECK(received != nullptr);
        CHECK(received->id == 7);
        CHECK(received->name == "Signal");
    }

    TEST_CASE("Emit and receive multiple complex types")
    {
        duin::Signal<std::vector<std::string>, CustomData, std::map<int, double>> sig;
        std::vector<std::string> vecReceived;
        CustomData structReceived;
        std::map<int, double> mapReceived;
        sig.Connect([&](const std::vector<std::string> &v, const CustomData &d, const std::map<int, double> &m) {
            vecReceived = v;
            structReceived = d;
            mapReceived = m;
        });
        std::vector<std::string> testVec = {"x", "y"};
        CustomData testData{99, "Complex"};
        std::map<int, double> testMap = {{1, 1.1}, {2, 2.2}};
        sig.Emit(testVec, testData, testMap);
        CHECK(vecReceived == testVec);
        CHECK(structReceived == testData);
        CHECK(mapReceived == testMap);
    }
}
} // namespace TestSignal
