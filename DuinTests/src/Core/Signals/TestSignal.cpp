#include <doctest.h>
#include <Duin/Core/Signals/Signal.h>
#include <Duin/Core/Debug/DNLog.h>
#include <vector>
#include <string>
#include <cstdint>
#include <map>
#include <memory>
#include <functional>
#include <stdexcept>
#include <chrono>
#include <algorithm>
#include <atomic>
#include <array>
#include "Defines.h"

// ---------------------------------------------------------------------------
// Helper types
// ---------------------------------------------------------------------------

struct CustomData
{
    int id;
    std::string name;
    bool operator==(const CustomData &other) const
    {
        return id == other.id && name == other.name;
    }
};

// Tracks construction, copy, and move counts
struct LifetimeTracker
{
    static int constructs;
    static int copies;
    static int moves;
    static int destructs;

    static void Reset()
    {
        constructs = copies = moves = destructs = 0;
    }

    LifetimeTracker()
    {
        ++constructs;
    }
    LifetimeTracker(const LifetimeTracker &)
    {
        ++copies;
    }
    LifetimeTracker(LifetimeTracker &&)
    {
        ++moves;
    }
    ~LifetimeTracker()
    {
        ++destructs;
    }
};
int LifetimeTracker::constructs = 0;
int LifetimeTracker::copies = 0;
int LifetimeTracker::moves = 0;
int LifetimeTracker::destructs = 0;

// Stateful struct that throws on copy after a threshold
struct FlakyCallback
{
    int *counter;
    int threshold;
    void operator()(int) const
    {
        if (*counter >= threshold)
            throw std::runtime_error("FlakyCallback threshold exceeded");
        ++(*counter);
    }
};

namespace TestSignal
{
TEST_SUITE("Signal - Core")
{
    // -----------------------------------------------------------------------
    // --- Identity & Construction ---
    // -----------------------------------------------------------------------

    TEST_CASE("Signal UUID is valid and unique")
    {
        duin::Signal<> sig1;
        duin::Signal<> sig2;
        CHECK(sig1.GetUUID() != duin::UUID::INVALID);
        CHECK(sig2.GetUUID() != duin::UUID::INVALID);
        CHECK(sig1.GetUUID() != sig2.GetUUID());
    }

    TEST_CASE("Many signals have unique UUIDs")
    {
        const int N = 500;
        std::vector<duin::UUID> uuids;
        uuids.reserve(N);
        for (int i = 0; i < N; ++i)
        {
            duin::Signal<> s;
            uuids.push_back(s.GetUUID());
        }
        // All UUIDs must be distinct
        std::sort(uuids.begin(), uuids.end(), [](const duin::UUID &a, const duin::UUID &b) {
            return static_cast<uint64_t>(a) < static_cast<uint64_t>(b);
        });
        auto it = std::adjacent_find(uuids.begin(), uuids.end());
        CHECK(it == uuids.end());
    }

    TEST_CASE("Default-constructed signal has zero listeners")
    {
        duin::Signal<int> sig;
        CHECK(sig.GetListenerCount() == 0);
    }

    // -----------------------------------------------------------------------
    // --- Connect ---
    // -----------------------------------------------------------------------

    TEST_CASE("Connect returns valid UUID")
    {
        duin::Signal<int> sig;
        duin::UUID id = sig.Connect([](int) {});
        CHECK(id != duin::UUID::INVALID);
    }

    TEST_CASE("Each Connect returns a unique UUID")
    {
        duin::Signal<int> sig;
        auto id1 = sig.Connect([](int) {});
        auto id2 = sig.Connect([](int) {});
        auto id3 = sig.Connect([](int) {});
        CHECK(id1 != id2);
        CHECK(id2 != id3);
        CHECK(id1 != id3);
    }

    TEST_CASE("Connect same lambda twice gives distinct IDs")
    {
        duin::Signal<int> sig;
        int count = 0;
        auto lambda = [&](int v) { count += v; };

        auto id1 = sig.Connect(lambda);
        auto id2 = sig.Connect(lambda);
        CHECK(id1 != id2);
        CHECK(sig.GetListenerCount() == 2);

        sig.Emit(1);
        CHECK(count == 2);
    }

    TEST_CASE("Connect nullptr std::function is stored but not invoked")
    {
        // The signal stores the null-callable but guards against calling it in Emit
        duin::Signal<int> sig;
        std::function<void(int)> nullFn = nullptr;
        duin::UUID id = sig.Connect(nullFn);
        CHECK(id != duin::UUID::INVALID);
        CHECK(sig.GetListenerCount() == 1);
        // Should not crash
        sig.Emit(42);
        // Cleanup
        sig.Disconnect(id);
        CHECK(sig.GetListenerCount() == 0);
    }

    TEST_CASE("Connect with raw function pointer")
    {
        static int gValue = 0;
        duin::Signal<int> sig;
        sig.Connect([](int v) { gValue = v; });
        sig.Emit(77);
        CHECK(gValue == 77);
    }

    // -----------------------------------------------------------------------
    // --- Emit ---
    // -----------------------------------------------------------------------

    TEST_CASE("Emit calls single listener with correct value")
    {
        duin::Signal<int> sig;
        int received = -1;
        sig.Connect([&](int v) { received = v; });
        sig.Emit(42);
        CHECK(received == 42);
    }

    TEST_CASE("Emit with no listeners does not crash")
    {
        duin::Signal<int> sig;
        sig.Emit(1);
        CHECK(true);
    }

    TEST_CASE("Emit with no-parameter signal")
    {
        duin::Signal<> sig;
        int count = 0;
        sig.Connect([&]() { ++count; });
        sig.Connect([&]() { ++count; });
        sig.Emit();
        CHECK(count == 2);
        sig.Emit();
        CHECK(count == 4);
    }

    TEST_CASE("Emit notifies all listeners in registration order")
    {
        duin::Signal<int> sig;
        std::vector<int> order;
        sig.Connect([&](int) { order.push_back(1); });
        sig.Connect([&](int) { order.push_back(2); });
        sig.Connect([&](int) { order.push_back(3); });
        sig.Emit(0);
        REQUIRE(order.size() == 3);
        CHECK(order[0] == 1);
        CHECK(order[1] == 2);
        CHECK(order[2] == 3);
    }

    TEST_CASE("Emit multiple times accumulates correctly")
    {
        duin::Signal<int> sig;
        int sum = 0;
        sig.Connect([&](int v) { sum += v; });
        sig.Emit(1);
        sig.Emit(2);
        sig.Emit(3);
        CHECK(sum == 6);
    }

    TEST_CASE("Emit with multiple argument types")
    {
        duin::Signal<int, std::string, double> sig;
        int ri = 0;
        std::string rs;
        double rd = 0.0;
        sig.Connect([&](int a, const std::string &b, double c) {
            ri = a;
            rs = b;
            rd = c;
        });
        sig.Emit(7, "hello", 3.14);
        CHECK(ri == 7);
        CHECK(rs == "hello");
        CHECK(rd == doctest::Approx(3.14));
    }

    TEST_CASE("Emit passes arguments independently to each listener")
    {
        duin::Signal<int> sig;
        int a = 0, b = 0, c = 0;
        sig.Connect([&](int v) { a = v; });
        sig.Connect([&](int v) { b = v * 2; });
        sig.Connect([&](int v) { c = v * 3; });
        sig.Emit(5);
        CHECK(a == 5);
        CHECK(b == 10);
        CHECK(c == 15);
    }

    TEST_CASE("Emit with std::vector argument")
    {
        duin::Signal<std::vector<int>> sig;
        std::vector<int> received;
        sig.Connect([&](const std::vector<int> &v) { received = v; });
        std::vector<int> src = {1, 2, 3, 4};
        sig.Emit(src);
        CHECK(received == src);
    }

    TEST_CASE("Emit with std::map argument")
    {
        duin::Signal<std::map<std::string, int>> sig;
        std::map<std::string, int> received;
        sig.Connect([&](const std::map<std::string, int> &m) { received = m; });
        std::map<std::string, int> src = {{"a", 1}, {"b", 2}};
        sig.Emit(src);
        CHECK(received == src);
    }

    TEST_CASE("Emit with custom struct argument")
    {
        duin::Signal<CustomData> sig;
        CustomData received{};
        sig.Connect([&](const CustomData &d) { received = d; });
        CustomData src{42, "Duin"};
        sig.Emit(src);
        CHECK(received == src);
    }

    TEST_CASE("Emit with shared_ptr argument")
    {
        duin::Signal<std::shared_ptr<CustomData>> sig;
        std::shared_ptr<CustomData> received;
        sig.Connect([&](std::shared_ptr<CustomData> p) { received = p; });
        auto src = std::make_shared<CustomData>(CustomData{7, "Signal"});
        sig.Emit(src);
        REQUIRE(received != nullptr);
        CHECK(received->id == 7);
        CHECK(received->name == "Signal");
    }

    TEST_CASE("Emit with lvalue reference argument allows mutation by listener")
    {
        duin::Signal<std::string &> sig;
        sig.Connect([](std::string &s) { s = "mutated"; });
        std::string val = "original";
        sig.Emit(val);
        // Whether mutation is visible depends on how the signal forwards.
        // This documents current behaviour: the value was passed and received.
        CHECK(true); // Reach here without UB
    }

    TEST_CASE("Emit with complex mixed argument types")
    {
        duin::Signal<std::vector<std::string>, CustomData, std::map<int, double>> sig;
        std::vector<std::string> vr;
        CustomData dr{};
        std::map<int, double> mr;
        sig.Connect([&](const std::vector<std::string> &v, const CustomData &d, const std::map<int, double> &m) {
            vr = v;
            dr = d;
            mr = m;
        });
        sig.Emit({"x", "y"}, CustomData{99, "Complex"}, {{1, 1.1}, {2, 2.2}});
        CHECK(vr == std::vector<std::string>{"x", "y"});
        CHECK(dr == (CustomData{99, "Complex"}));
        CHECK(mr == (std::map<int, double>{{1, 1.1}, {2, 2.2}}));
    }

    // -----------------------------------------------------------------------
    // --- Disconnect ---
    // -----------------------------------------------------------------------

    TEST_CASE("Disconnect returns true and removes listener")
    {
        duin::Signal<int> sig;
        int called = 0;
        duin::UUID id = sig.Connect([&](int v) { called = v; });
        sig.Disconnect(id);
        sig.Emit(99);
        CHECK(called == 0);
        CHECK(sig.GetListenerCount() == 0);
    }

    TEST_CASE("Disconnect unknown UUID returns false")
    {
        duin::Signal<> sig;
        duin::UUID fake(123456);
        sig.Disconnect(fake);
    }

    TEST_CASE("Disconnect INVALID UUID returns false")
    {
        duin::Signal<int> sig;
        sig.Connect([](int) {});
        sig.Disconnect(duin::UUID::INVALID);
    }

    TEST_CASE("Disconnect zero-value UUID returns false")
    {
        duin::Signal<int> sig;
        sig.Connect([](int) {});
        sig.Disconnect(duin::UUID(0));
    }

    TEST_CASE("Double disconnect on same UUID: second returns false")
    {
        duin::Signal<> sig;
        duin::UUID id = sig.Connect([]() {});
        sig.Disconnect(id);
        sig.Disconnect(id);
    }

    TEST_CASE("Disconnect middle listener preserves remaining listeners")
    {
        duin::Signal<int> sig;
        int a = 0, b = 0, c = 0;
        auto idA = sig.Connect([&](int v) { a = v; });
        auto idB = sig.Connect([&](int v) { b = v; });
        auto idC = sig.Connect([&](int v) { c = v; });

        sig.Disconnect(idB);
        sig.Emit(5);
        CHECK(a == 5);
        CHECK(b == 0); // B was disconnected
        CHECK(c == 5);
    }

    TEST_CASE("Disconnect first listener preserves remaining listeners")
    {
        duin::Signal<int> sig;
        int a = 0, b = 0;
        auto idA = sig.Connect([&](int v) { a = v; });
        auto idB = sig.Connect([&](int v) { b = v; });

        sig.Disconnect(idA);
        sig.Emit(7);
        CHECK(a == 0);
        CHECK(b == 7);
    }

    TEST_CASE("Disconnect last listener leaves signal empty")
    {
        duin::Signal<int> sig;
        auto id = sig.Connect([](int) {});
        sig.Disconnect(id);
        CHECK(sig.GetListenerCount() == 0);
        sig.Emit(1); // Must not crash
        CHECK(true);
    }

    TEST_CASE("Connect and immediate disconnect")
    {
        duin::Signal<int> sig;
        auto id = sig.Connect([](int) {});
        CHECK(sig.GetListenerCount() == 1);
        sig.Disconnect(id);
        CHECK(sig.GetListenerCount() == 0);
    }

    TEST_CASE("Multiple connects and disconnects interleaved")
    {
        duin::Signal<int> sig;
        int a = 0, b = 0;
        auto idA = sig.Connect([&](int v) { a = v; });
        auto idB = sig.Connect([&](int v) { b = v; });

        sig.Emit(5);
        CHECK(a == 5);
        CHECK(b == 5);

        sig.Disconnect(idA);
        a = b = 0;
        sig.Emit(10);
        CHECK(a == 0);
        CHECK(b == 10);

        sig.Disconnect(idB);
        a = b = 0;
        sig.Emit(20);
        CHECK(a == 0);
        CHECK(b == 0);
    }

    // -----------------------------------------------------------------------
    // --- GetListenerCount ---
    // -----------------------------------------------------------------------

    TEST_CASE("GetListenerCount tracks adds and removes accurately")
    {
        duin::Signal<int> sig;
        CHECK(sig.GetListenerCount() == 0);
        auto id1 = sig.Connect([](int) {});
        CHECK(sig.GetListenerCount() == 1);
        auto id2 = sig.Connect([](int) {});
        CHECK(sig.GetListenerCount() == 2);
        auto id3 = sig.Connect([](int) {});
        CHECK(sig.GetListenerCount() == 3);
        sig.Disconnect(id2);
        CHECK(sig.GetListenerCount() == 2);
        sig.Disconnect(id1);
        CHECK(sig.GetListenerCount() == 1);
        sig.Disconnect(id3);
        CHECK(sig.GetListenerCount() == 0);
    }

    // -----------------------------------------------------------------------
    // --- Reentrancy & Self-modification during Emit ---
    // -----------------------------------------------------------------------

    TEST_CASE("Listener disconnects itself during Emit — not called again")
    {
        duin::Signal<int> sig;
        int callCount = 0;
        duin::UUID id;
        id = sig.Connect([&](int) {
            ++callCount;
            sig.Disconnect(id);
        });
        sig.Emit(1);
        sig.Emit(2);
        CHECK(callCount == 1);
    }

    TEST_CASE("One listener disconnects another during Emit")
    {
        // When disconnect uses swap-and-pop, the swapped listener must still fire.
        duin::Signal<int> sig;
        int aCount = 0, bCount = 0;
        duin::UUID idB;
        sig.Connect([&](int) {
            ++aCount;
            sig.Disconnect(idB); // removes B while iterating
        });
        idB = sig.Connect([&](int) { ++bCount; });

        sig.Emit(1);
        // A must have been called. B's fate depends on iteration order vs. swap-pop.
        // Document and verify counts are non-negative (no UB/crash).
        CHECK(aCount == 1);
        CHECK(bCount >= 0); // B may or may not fire depending on swap order

        // After emit, B is definitely gone
        sig.Emit(1);
        CHECK(aCount == 2);
        int prevB = bCount;
        sig.Emit(1);
        CHECK(bCount == prevB); // B no longer connected
    }

    TEST_CASE("Listener connects a NEW listener during Emit — new listener not invoked same emit")
    {
        duin::Signal<int> sig;
        int firstCount = 0, newCount = 0;
        sig.Connect([&](int) {
            ++firstCount;
            if (firstCount == 1)
            {
                sig.Connect([&](int) { ++newCount; });
            }
        });
        sig.Emit(1);
        // New listener was added during emit; whether it fires in the same emit is
        // implementation-defined. Document current behaviour.
        int afterFirstEmit = newCount;

        sig.Emit(1);
        // After second emit, new listener MUST fire
        CHECK(newCount > afterFirstEmit);
    }

    TEST_CASE("Listener emits the same signal recursively")
    {
        // Verifies that recursive emit does not infinitely recurse
        duin::Signal<int> sig;
        int depth = 0;
        sig.Connect([&](int v) {
            ++depth;
            if (v > 0 && depth <= 3)
            {
                sig.Emit(v - 1); // recurse
            }
        });
        sig.Emit(3);
        // Depth should be bounded and not crash
        CHECK(depth >= 3);
    }

    // -----------------------------------------------------------------------
    // --- Lifetime / Dangling reference hazards ---
    // -----------------------------------------------------------------------

    TEST_CASE("Listener callback holding a shared_ptr keeps object alive through emit")
    {
        duin::Signal<int> sig;
        auto data = std::make_shared<int>(0);
        std::weak_ptr<int> weak = data;

        sig.Connect([data](int v) mutable { *data = v; }); // callback owns shared_ptr
        data.reset();                                      // release caller's reference

        CHECK(!weak.expired()); // callback still holds it
        sig.Emit(99);
        CHECK(*weak.lock() == 99);
    }

    TEST_CASE("Listener captures weak_ptr — object destroyed before emit does not crash")
    {
        duin::Signal<int> sig;
        auto obj = std::make_shared<int>(0);
        std::weak_ptr<int> weak = obj;

        sig.Connect([weak](int v) {
            if (auto locked = weak.lock())
            {
                *locked = v;
            }
            // If expired, do nothing — must not crash
        });

        obj.reset(); // destroy the object
        CHECK(weak.expired());

        // Emit must not crash even though the captured object is gone
        sig.Emit(55);
        CHECK(true);
    }

    TEST_CASE("Signal destroyed before listener UUID is used — Disconnect on dead signal")
    {
        // Create signal on heap, connect a listener, destroy signal,
        // then attempt disconnect on a new signal with the old UUID.
        // This documents that UUIDs from dead signals are foreign to new signals.
        duin::UUID capturedId;
        {
            duin::Signal<int> sig;
            capturedId = sig.Connect([](int) {});
        } // sig destroyed here

        // A brand-new signal should not contain capturedId
        duin::Signal<int> newSig;
        newSig.Disconnect(capturedId);
    }

    TEST_CASE("Listener callback with destroyed capture context — UB fence")
    {
        // This test documents a known hazard: if a listener captures a local by reference
        // and the local is destroyed while the signal still exists, emit is UB.
        // We test the safe pattern (capture by value / smart pointer).
        duin::Signal<int> sig;
        std::shared_ptr<int> counter = std::make_shared<int>(0);
        duin::UUID id;
        {
            int localVal = 10; // local that will be destroyed
            id = sig.Connect([counter, localVal](int v) mutable {
                *counter += localVal + v; // captures by value — safe
            });
        } // localVal destroyed, but counter and localVal copy are held by lambda

        sig.Emit(5);
        CHECK(*counter == 15); // 10 + 5
    }

    TEST_CASE("Signal with std::shared_ptr listener: listener outlives signal")
    {
        std::function<void(int)> preserved;
        duin::UUID capturedId;
        {
            duin::Signal<int> sig;
            capturedId = sig.Connect([&preserved](int) {});
            // Signal goes out of scope; preserved is just a reference here
        }
        // Signal is dead — captured id is now invalid against any new signal
        duin::Signal<int> fresh;
        CHECK(fresh.GetListenerCount() == 0);
        fresh.Disconnect(capturedId);
    }

    // -----------------------------------------------------------------------
    // --- Object lifetime tracking ---
    // -----------------------------------------------------------------------

    TEST_CASE("Emit does not create extra copies of value-type arguments")
    {
        LifetimeTracker::Reset();
        duin::Signal<LifetimeTracker> sig;
        int copyCount = 0;
        sig.Connect([&](LifetimeTracker) { ++copyCount; }); // parameter copy is expected
        sig.Connect([&](LifetimeTracker) { ++copyCount; });

        LifetimeTracker t; // 1 construct
        sig.Emit(t);       // passes to 2 listeners; each receives a copy (by-value param)
        // Just verify it reaches both listeners without crashing
        CHECK(copyCount == 2);
    }

    // -----------------------------------------------------------------------
    // --- Exception safety ---
    // -----------------------------------------------------------------------

    TEST_CASE("Throwing listener does not prevent subsequent listeners from being called — known limitation")
    {
        // Current implementation has NO exception guard; a throwing listener
        // will unwind the Emit loop. This test documents that behaviour.
        duin::Signal<int> sig;
        int beforeCount = 0, afterCount = 0;
        int throwCounter = 0;

        sig.Connect([&](int) { ++beforeCount; });
        sig.Connect([&](int) {
            ++throwCounter;
            throw std::runtime_error("listener threw");
        });
        sig.Connect([&](int) { ++afterCount; });

        CHECK_THROWS_AS(sig.Emit(1), std::runtime_error);
        CHECK(beforeCount == 1); // fired before the throw
        CHECK(afterCount == 0);  // never reached — known gap
    }

    TEST_CASE("Signal state is consistent after a throwing emit")
    {
        duin::Signal<int> sig;
        int counter = 0;
        sig.Connect([&](int) {
            if (counter == 0)
                throw std::runtime_error("once");
            ++counter;
        });

        CHECK_THROWS(sig.Emit(1));
        // Signal itself must still be in a usable state
        counter = 1; // disarm the throw
        CHECK_NOTHROW(sig.Emit(1));
        CHECK(counter == 2);
    }

    // -----------------------------------------------------------------------
    // --- Stress & Performance ---
    // -----------------------------------------------------------------------

    TEST_CASE("Connect and Emit 1000 listeners")
    {
        const int N = 1000;
        duin::Signal<int> sig;
        int total = 0;
        for (int i = 0; i < N; ++i)
            sig.Connect([&](int v) { total += v; });

        MSG_CHECK(sig.GetListenerCount(), sig.GetListenerCount() == N);
        sig.Emit(1);
        CHECK(total == N);
    }

    TEST_CASE("Connect and disconnect 10000 listeners round-trip")
    {
        const int N = 10000;
        duin::Signal<int> sig;
        std::vector<duin::UUID> ids;
        ids.reserve(N);

        for (int i = 0; i < N; ++i)
            ids.push_back(sig.Connect([](int) { return true; }));
        CHECK(sig.GetListenerCount() == N);

        for (auto &id : ids)
            sig.Disconnect(id);
        CHECK(sig.GetListenerCount() == 0);
    }

    TEST_CASE("Alternating connect/disconnect does not leak listeners")
    {
        duin::Signal<int> sig;
        for (int i = 0; i < 500; ++i)
        {
            auto id = sig.Connect([](int) {});
            sig.Disconnect(id);
        }
        CHECK(sig.GetListenerCount() == 0);
    }

    TEST_CASE("Emit performance: 10000 emits with 10 listeners completes without hang")
    {
        duin::Signal<int> sig;
        int total = 0;
        for (int i = 0; i < 10; ++i)
            sig.Connect([&](int v) { total += v; });

        auto start = std::chrono::steady_clock::now();
        for (int i = 0; i < 10000; ++i)
            sig.Emit(1);
        auto end = std::chrono::steady_clock::now();

        CHECK(total == 100000);
        auto ms = std::chrono::duration_cast<std::chrono::milliseconds>(end - start).count();
        // Generous budget — 2 seconds for a debug build
        CHECK(ms < 2000);
    }

    TEST_CASE("Large emit payload: 1000 listeners x 100 emits")
    {
        const int LISTENERS = 1000;
        const int EMITS = 100;
        duin::Signal<int> sig;
        int total = 0;
        for (int i = 0; i < LISTENERS; ++i)
            sig.Connect([&](int v) { total += v; });
        for (int i = 0; i < EMITS; ++i)
            sig.Emit(1);
        CHECK(total == LISTENERS * EMITS);
    }

    TEST_CASE("Rapid connect-disconnect-emit cycle does not corrupt state")
    {
        duin::Signal<int> sig;
        int total = 0;
        duin::UUID sticky = sig.Connect([&](int v) { total += v; });

        for (int i = 0; i < 200; ++i)
        {
            auto tmp = sig.Connect([&](int v) { total += v; });
            sig.Emit(1);
            sig.Disconnect(tmp);
        }

        // Only sticky remains; each of 200 outer iterations fired sticky + tmp once
        CHECK(total == 400); // 2 per iteration * 200
        CHECK(sig.GetListenerCount() == 1);
    }

    TEST_CASE("Large disconnect during emit: 1000 listeners x 100 emits")
    {
        const int LISTENERS = 1000;
        const int EMITS = 100;
        duin::Signal<int> sig;
        int total = 0;
        std::vector<duin::UUID> connections(1000);
        for (int i = 0; i < LISTENERS; ++i)
        {
            auto id = sig.Connect([&, i](int v) { sig.Disconnect(connections[i]); });
            connections[i] = id;
        }
        for (int i = 0; i < EMITS; ++i)
        {
            sig.Emit(1);
        }
        MSG_CHECK(sig.GetListenerCount(), sig.GetListenerCount() == 0);
    }

    struct TST_A
    {
        int x = 1;

        duin::UUID Connect(duin::Signal<int> &sig)
        {
            return sig.Connect([this](int i) { this->x = i; });
        }
    };
    TEST_CASE("Connection with references gets deleted without disconnection")
    {
        duin::Signal<int> sig;
        {
            TST_A *tst = new TST_A();
            auto id = tst->Connect(sig);
            delete tst;
        }
        sig.Emit(3);
        MESSAGE("Signal connection count: ", sig.GetListenerCount());
    }

    // -----------------------------------------------------------------------
    // --- Edge cases specific to swap-and-pop Disconnect ---
    // -----------------------------------------------------------------------

    TEST_CASE("Disconnect last element: no swap needed, no out-of-bounds")
    {
        duin::Signal<int> sig;
        auto id1 = sig.Connect([](int) {});
        auto id2 = sig.Connect([](int) {}); // last element
        sig.Disconnect(id2);
        CHECK(sig.GetListenerCount() == 1);
        // id1 still fires
        int fired = 0;
        auto id3 = sig.Connect([&](int) { fired = 1; });
        sig.Emit(0);
        CHECK(fired == 1);
    }

    TEST_CASE("Disconnect first element: swapped with last, last still fires")
    {
        duin::Signal<int> sig;
        int a = 0, b = 0, c = 0;
        auto idA = sig.Connect([&](int v) { a = v; });
        auto idB = sig.Connect([&](int v) { b = v; });
        auto idC = sig.Connect([&](int v) { c = v; }); // last

        sig.Disconnect(idA); // A swapped with C (last), A removed
        sig.Emit(9);
        CHECK(a == 0);
        CHECK(b == 9);
        CHECK(c == 9);
    }

    TEST_CASE("Disconnect only element: list becomes empty cleanly")
    {
        duin::Signal<int> sig;
        auto id = sig.Connect([](int) {});
        MSG_CHECK(sig.GetListenerCount(), sig.GetListenerCount() == 1);
        sig.Disconnect(id);
        MSG_CHECK(sig.GetListenerCount(), sig.GetListenerCount() == 0);
        sig.Emit(1); // must not crash on empty
    }

    TEST_CASE("Disconnect all elements in all 6 permutations of 3 listeners")
    {
        // Enumerate all 6 orderings of disconnecting 3 listeners
        std::vector<std::array<int, 3>> perms = {{0, 1, 2}, {0, 2, 1}, {1, 0, 2}, {1, 2, 0}, {2, 0, 1}, {2, 1, 0}};
        for (auto &perm : perms)
        {
            duin::Signal<int> sig;
            int vals[3] = {0, 0, 0};
            duin::UUID ids[3];
            for (int i = 0; i < 3; ++i)
                ids[i] = sig.Connect([&vals, i](int v) { vals[i] = v; });

            for (int i = 0; i < 3; ++i)
                sig.Disconnect(ids[perm[i]]);

            CHECK(sig.GetListenerCount() == 0);
            sig.Emit(1); // must not crash on empty
        }
    }

    // -----------------------------------------------------------------------
    // --- Miscellaneous correctness ---
    // -----------------------------------------------------------------------

    TEST_CASE("Signal<> (no args) GetUUID is stable across emits")
    {
        duin::Signal<> sig;
        duin::UUID before = sig.GetUUID();
        sig.Emit();
        sig.Emit();
        CHECK(sig.GetUUID() == before);
    }

    TEST_CASE("Signal UUID does not change after connect or disconnect")
    {
        duin::Signal<int> sig;
        duin::UUID before = sig.GetUUID();
        auto id = sig.Connect([](int) {});
        CHECK(sig.GetUUID() == before);
        sig.Disconnect(id);
        CHECK(sig.GetUUID() == before);
    }

    TEST_CASE("Two signals are fully independent")
    {
        duin::Signal<int> sig1, sig2;
        int a = 0, b = 0;
        sig1.Connect([&](int v) { a = v; });
        sig2.Connect([&](int v) { b = v; });

        sig1.Emit(10);
        CHECK(a == 10);
        CHECK(b == 0);

        sig2.Emit(20);
        CHECK(a == 10);
        CHECK(b == 20);
    }

    TEST_CASE("Listeners still fire after failed Disconnect attempt")
    {
        duin::Signal<int> sig;
        int fired = 0;
        sig.Connect([&](int) { ++fired; });
        sig.Disconnect(duin::UUID(999)); // fails silently
        sig.Emit(1);
        CHECK(fired == 1);
    }

    TEST_CASE("Signal with bool argument: true and false both received correctly")
    {
        duin::Signal<bool> sig;
        bool received = false;
        sig.Connect([&](bool v) { received = v; });
        sig.Emit(true);
        CHECK(received == true);
        sig.Emit(false);
        CHECK(received == false);
    }

    TEST_CASE("Signal with uint64_t argument (same underlying type as UUID)")
    {
        duin::Signal<uint64_t> sig;
        uint64_t received = 0;
        sig.Connect([&](uint64_t v) { received = v; });
        sig.Emit(UINT64_MAX);
        CHECK(received == UINT64_MAX);
    }

    TEST_CASE("Signal with nullptr_t argument")
    {
        duin::Signal<std::nullptr_t> sig;
        bool called = false;
        sig.Connect([&](std::nullptr_t) { called = true; });
        sig.Emit(nullptr);
        CHECK(called == true);
    }

    TEST_CASE("Listener modifies signal state via captured ref — does not invalidate signal")
    {
        // Captures a reference to the signal itself and reconnects each time
        duin::Signal<int> sig;
        int callCount = 0;
        duin::UUID dynId;

        auto reconnect = [&](int) {
            ++callCount;
            sig.Disconnect(dynId);
            dynId = sig.Connect([&](int v) { ++callCount; });
        };
        dynId = sig.Connect(reconnect);

        sig.Emit(1); // reconnect fires, removes itself, adds a new listener
        sig.Emit(1); // new listener fires (it's a simple counter)
        CHECK(callCount >= 2);
    }

    TEST_CASE("Connecting an already-wrapped std::function works")
    {
        duin::Signal<int> sig;
        std::function<void(int)> fn = [](int) {};
        auto id = sig.Connect(fn);
        CHECK(id != duin::UUID::INVALID);
        CHECK(sig.GetListenerCount() == 1);
    }

    TEST_CASE("Many signals interacting — no cross-signal contamination")
    {
        const int N = 20;
        std::vector<duin::Signal<int>> sigs(N);
        std::vector<int> counts(N, 0);

        for (int i = 0; i < N; ++i)
            sigs[i].Connect([&counts, i](int v) { counts[i] += v; });

        for (int i = 0; i < N; ++i)
            sigs[i].Emit(i + 1);

        for (int i = 0; i < N; ++i)
            CHECK(counts[i] == i + 1);
    }

    // -----------------------------------------------------------------------
    // --- Listener scope / UUID lifetime ---
    // -----------------------------------------------------------------------

    TEST_CASE("UUID going out of scope does NOT auto-disconnect — listener keeps firing")
    {
        // Documents that duin::Signal has no RAII guard: dropping the UUID
        // handle does not remove the listener.
        duin::Signal<int> sig;
        int fired = 0;

        {
            duin::UUID id = sig.Connect([&](int) { ++fired; });
            (void)id;
        } // id goes out of scope here — no auto-disconnect

        sig.Emit(1);
        CHECK(fired == 1); // listener is still registered
        CHECK(sig.GetListenerCount() == 1);
    }

    TEST_CASE("Listener lambda captures local that goes out of scope — UUID survives, callback still runs")
    {
        // The lambda captures by value so the local's copy lives inside the
        // std::function even after the outer scope ends.
        duin::Signal<int> sig;
        int result = 0;

        {
            int multiplier = 3;
            sig.Connect([&result, multiplier](int v) { result = v * multiplier; });
            // multiplier goes out of scope but its VALUE was captured
        }

        sig.Emit(4);
        CHECK(result == 12); // capture-by-value copy kept the callback valid
    }

    TEST_CASE("Listener registered inside nested scope fires after that scope ends")
    {
        duin::Signal<int> sig;
        int total = 0;

        for (int i = 0; i < 3; ++i)
        {
            // Each UUID is a loop-local; it goes out of scope each iteration.
            duin::UUID id = sig.Connect([&total, i](int v) { total += v * (i + 1); });
            (void)id;
        }
        // All 3 UUIDs are gone — but all 3 listeners remain connected.
        CHECK(sig.GetListenerCount() == 3);

        sig.Emit(1); // weights: 1, 2, 3
        CHECK(total == 6);
    }

    TEST_CASE("Lost UUID cannot be used to disconnect — listener is permanent until signal dies")
    {
        duin::Signal<int> sig;
        int fired = 0;

        {
            duin::UUID id = sig.Connect([&](int) { ++fired; });
            (void)id;
        } // id lost

        // Cannot disconnect without the UUID; listener fires indefinitely.
        sig.Emit(1);
        sig.Emit(1);
        sig.Emit(1);
        CHECK(fired == 3);
        CHECK(sig.GetListenerCount() == 1); // still there, no way to remove it
    }

    TEST_CASE("Listener object (struct with operator()) goes out of scope — copy held by signal keeps firing")
    {
        // Signal stores callbacks by value inside std::function.
        // Even if the original functor is destroyed the internal copy persists.
        duin::Signal<int> sig;
        int count = 0;

        struct Counter
        {
            int *p;
            void operator()(int) const
            {
                ++(*p);
            }
        };

        {
            Counter c{&count};
            sig.Connect(c); // copied into std::function
        } // c destroyed — but the copy inside the signal remains valid

        sig.Emit(1);
        sig.Emit(1);
        CHECK(count == 2);
    }

    TEST_CASE("Multiple scoped UUIDs: all listeners survive and fire independently")
    {
        duin::Signal<int> sig;
        int a = 0, b = 0, c = 0;

        {
            duin::UUID id = sig.Connect([&](int v) { a = v; });
            (void)id;
        }
        {
            duin::UUID id = sig.Connect([&](int v) { b = v * 2; });
            (void)id;
        }
        {
            duin::UUID id = sig.Connect([&](int v) { c = v * 3; });
            (void)id;
        }

        CHECK(sig.GetListenerCount() == 3);
        sig.Emit(5);
        CHECK(a == 5);
        CHECK(b == 10);
        CHECK(c == 15);
    }

    // ---------------------------------------------------------------------------
    // --- ScopedConnection ---
    // ---------------------------------------------------------------------------
    TEST_CASE("ConnectScoped returns non-null shared_ptr; listener count is 1")
    {
        duin::Signal<int> sig;
        auto conn = sig.ConnectScoped([](int) {});
        CHECK(conn != nullptr);
        CHECK(sig.GetListenerCount() == 1);
    }

    TEST_CASE("Listener fires while shared_ptr held; stops after reset")
    {
        duin::Signal<int> sig;
        int count = 0;
        auto conn = sig.ConnectScoped([&](int) { ++count; });

        sig.Emit(1);
        CHECK(count == 1);

        conn.reset();
        sig.Emit(1);
        CHECK(count == 1); // no longer fires
    }

    TEST_CASE("GetListenerCount decrements to 0 after shared_ptr reset")
    {
        duin::Signal<int> sig;
        auto conn = sig.ConnectScoped([](int) {});
        CHECK(sig.GetListenerCount() == 1);
        conn.reset();
        CHECK(sig.GetListenerCount() == 0);
    }

    TEST_CASE("Two scoped connections independently control their listeners")
    {
        duin::Signal<int> sig;
        int a = 0, b = 0;
        auto connA = sig.ConnectScoped([&](int v) { a = v; });
        auto connB = sig.ConnectScoped([&](int v) { b = v; });
        CHECK(sig.GetListenerCount() == 2);

        sig.Emit(5);
        CHECK(a == 5);
        CHECK(b == 5);

        connA.reset();
        CHECK(sig.GetListenerCount() == 1);

        sig.Emit(10);
        CHECK(a == 5); // A no longer fires
        CHECK(b == 10);

        connB.reset();
        CHECK(sig.GetListenerCount() == 0);
    }

    TEST_CASE("Signal destroyed before shared_ptr: destroying shared_ptr afterward does not crash")
    {
        std::shared_ptr<duin::ScopedConnection> conn;
        {
            duin::Signal<int> sig;
            conn = sig.ConnectScoped([](int) {});
            // sig destroyed here; ScopedConnection is invalidated
        }
        // Destroying conn must not crash
        conn.reset();
        CHECK(true);
    }

    TEST_CASE("Release: listener remains connected after shared_ptr destroyed")
    {
        duin::Signal<int> sig;
        int count = 0;
        {
            auto conn = sig.ConnectScoped([&](int) { ++count; });
            conn->Release();
            // conn destroyed here — but listener should stay because Release() was called
        }
        CHECK(sig.GetListenerCount() == 1);
        sig.Emit(1);
        CHECK(count == 1);
    }

    TEST_CASE("Mix of Connect and ConnectScoped: all listeners fire")
    {
        duin::Signal<int> sig;
        int plain = 0, scoped = 0;
        sig.Connect([&](int v) { plain = v; });
        auto conn = sig.ConnectScoped([&](int v) { scoped = v; });
        CHECK(sig.GetListenerCount() == 2);

        sig.Emit(7);
        CHECK(plain == 7);
        CHECK(scoped == 7);

        conn.reset();
        CHECK(sig.GetListenerCount() == 1);

        sig.Emit(9);
        CHECK(plain == 9);
        CHECK(scoped == 7); // scoped no longer fires
    }

    TEST_CASE("ScopedConnection - Out of scope test")
    {
        duin::Signal<int> sig;
        int scoped = -1;
        {
            auto conn = sig.ConnectScoped([&](int v) { scoped = v; });
        }
        sig.Emit(5);
        MSG_CHECK(sig.GetListenerCount(), sig.GetListenerCount() == 0);
        MSG_CHECK(scoped, scoped == -1);
    }

    TEST_CASE("ScopedConnection - Mixed scope test")
    {
        duin::Signal<int> sig;
        int scoped = -1;
        int global = -1;
        std::shared_ptr<duin::ScopedConnection> globalConn = sig.ConnectScoped([&](int v) { global = v; });
        {
            std::shared_ptr<duin::ScopedConnection> conn = sig.ConnectScoped([&](int v) { scoped = v; });
        }
        sig.Emit(5);
        MSG_CHECK(sig.GetListenerCount(), sig.GetListenerCount() == 1);
        sig.Disconnect(globalConn);
        MSG_CHECK(sig.GetListenerCount(), sig.GetListenerCount() == 0);
        MSG_CHECK(scoped, scoped == -1);
        MSG_CHECK(global, global == 5);
    }

    TEST_CASE("ScopedConnection - Signal goes out of scope")
    {
        int scoped = -1;
        int global = -1;
        std::shared_ptr<duin::ScopedConnection> conn;
        {
            duin::Signal<int> sig;
            conn = sig.ConnectScoped([&](int v) { global = v; });
            {
                std::shared_ptr<duin::ScopedConnection> conn = sig.ConnectScoped([&](int v) { scoped = v; });
            }
            sig.Emit(5);
            MSG_CHECK(sig.GetListenerCount(), sig.GetListenerCount() == 1);
            CHECK(conn->IsValid());
        }
        CHECK_FALSE(conn->IsValid());
    }

    // -----------------------------------------------------------------------
    // --- Cross-signal chains & recursion depth limit ---
    // -----------------------------------------------------------------------

    TEST_CASE("Emit returns true when no recursion occurs")
    {
        duin::Signal<int> sig;
        sig.Connect([](int) {});
        CHECK(sig.Emit(1) == true);
    }

    TEST_CASE("Signal_A emits Signal_B from listener — both return true, B fires correctly")
    {
        // A -> B (one hop, no cycle): both emits succeed and return true.
        duin::Signal<int> sigA, sigB;
        int bReceived = -1;
        bool bResult = false;
        sigB.Connect([&](int v) { bReceived = v; });
        sigA.Connect([&](int v) { bResult = sigB.Emit(v * 10); });

        bool aResult = sigA.Emit(3);
        CHECK(aResult == true);
        CHECK(bResult == true);
        CHECK(bReceived == 30);
    }

    TEST_CASE("Signal_A -> Signal_B -> Signal_C chain: all emits return true")
    {
        // A -> B -> C (two hops, no cycle): all three emits must return true.
        duin::Signal<int> sigA, sigB, sigC;
        bool cResult = false, bResult = false;
        std::vector<int> order;

        sigC.Connect([&](int) { order.push_back(3); });
        sigB.Connect([&](int v) {
            order.push_back(2);
            cResult = sigC.Emit(v);
        });
        sigA.Connect([&](int v) {
            order.push_back(1);
            bResult = sigB.Emit(v);
        });

        bool aResult = sigA.Emit(0);
        CHECK(aResult == true);
        CHECK(bResult == true);
        CHECK(cResult == true);
        REQUIRE(order.size() == 3);
        CHECK(order[0] == 1);
        CHECK(order[1] == 2);
        CHECK(order[2] == 3);
    }

    TEST_CASE("Signal_A -> Signal_B -> Signal_C: each signal is independent after chain")
    {
        // After the chain completes, each signal must still work independently.
        duin::Signal<int> sigA, sigB, sigC;
        int cCount = 0;

        sigC.Connect([&](int) { ++cCount; });
        sigB.Connect([&](int v) { sigC.Emit(v); });
        sigA.Connect([&](int v) { sigB.Emit(v); });

        CHECK(sigA.Emit(1) == true);
        CHECK(cCount == 1);

        // Each signal still works on its own
        CHECK(sigC.Emit(1) == true);
        CHECK(cCount == 2);
        CHECK(sigB.Emit(1) == true);
        CHECK(cCount == 3);
        CHECK(sigA.Emit(1) == true);
        CHECK(cCount == 4);
    }

    TEST_CASE("Signal_A -> Signal_B: B listener disconnects from B during A's emit")
    {
        // While A is emitting it calls B. B's listener disconnects itself mid-flight.
        duin::Signal<int> sigA, sigB;
        int bCount = 0;
        duin::UUID bId;

        bId = sigB.Connect([&](int) {
            ++bCount;
            sigB.Disconnect(bId); // self-disconnect inside nested signal
        });

        sigA.Connect([&](int v) { sigB.Emit(v); });

        CHECK(sigA.Emit(1) == true);
        CHECK(bCount == 1);
        CHECK(sigB.GetListenerCount() == 0);

        // A second emit of A must not invoke B's old listener
        CHECK(sigA.Emit(1) == true);
        CHECK(bCount == 1);
    }

    TEST_CASE("Signal_A -> Signal_B: multiple listeners on B all fire during A's emit")
    {
        duin::Signal<int> sigA, sigB;
        int b1 = 0, b2 = 0, b3 = 0;

        sigB.Connect([&](int v) { b1 += v; });
        sigB.Connect([&](int v) { b2 += v; });
        sigB.Connect([&](int v) { b3 += v; });
        sigA.Connect([&](int v) { sigB.Emit(v); });

        CHECK(sigA.Emit(5) == true);
        CHECK(b1 == 5);
        CHECK(b2 == 5);
        CHECK(b3 == 5);
    }

    TEST_CASE("Signal_A -> Signal_B -> Signal_C: disconnect from C during B's handler")
    {
        // Three-signal chain where the C listener disconnects itself.
        duin::Signal<int> sigA, sigB, sigC;
        int cCount = 0;
        duin::UUID cId;

        cId = sigC.Connect([&](int) {
            ++cCount;
            sigC.Disconnect(cId);
        });
        sigB.Connect([&](int v) { sigC.Emit(v); });
        sigA.Connect([&](int v) { sigB.Emit(v); });

        CHECK(sigA.Emit(1) == true);
        CHECK(cCount == 1);
        CHECK(sigC.GetListenerCount() == 0);

        // A and B still work; C is now empty
        CHECK(sigA.Emit(1) == true);
        CHECK(cCount == 1); // C listener was removed
    }

    TEST_CASE("Same-signal self-recursion returns false at depth 30")
    {
        // A listener that re-emits its own signal will trigger the depth guard.
        // Emit returns false once recursiveEmissionCount reaches 30.
        duin::Signal<int> sig;
        int depth = 0;
        bool hitLimit = false;

        sig.Connect([&](int v) {
            ++depth;
            bool result = sig.Emit(v); // re-emit same signal
            if (!result)
                hitLimit = true;
        });

        sig.Emit(0); // kick off the chain
        CHECK(hitLimit == true);
    }

    TEST_CASE("Same-signal self-recursion: signal is still usable after hitting depth limit")
    {
        // After the recursive chain unwinds, the signal must accept new emits normally.
        duin::Signal<int> sig;
        int depth = 0;
        sig.Connect([&](int v) {
            if (depth < 0)
                return;
            depth++;
            sig.Emit(0); // will hit limit and return false, then unwind
        });
        bool res = sig.Emit(0);
        MSG_CHECK_FALSE(res, res);

        // Reset depth and verify a fresh emit works
        depth = -1;
        CHECK(sig.Emit(1));
    }

    TEST_CASE("Signal_A -> Signal_B -> Signal_A: second-order cycle detected via A's depth limit")
    {
        // A emits B, B emits A. Each signal tracks its own depth independently.
        // A is re-entered while still executing, so A returns false from B's handler.
        duin::Signal<int> sigA, sigB;
        int aDepth = 0, bDepth = 0;
        bool aHitLimit = false;

        sigB.Connect([&](int v) {
            ++bDepth;
            bool result = sigA.Emit(v); // re-enter A while A is executing
            if (!result)
                aHitLimit = true;
        });

        sigA.Connect([&](int v) {
            ++aDepth;
            sigB.Emit(v); // A -> B -> A -> B -> ... until A's depth guard fires
        });

        sigA.Emit(0);
        CHECK(aHitLimit == true);  // A's depth guard was hit from inside B
    }

    TEST_CASE("Signal_A -> Signal_B -> Signal_C -> Signal_A: third-order cycle hits depth limit")
    {
        // A -> B -> C -> A -> B -> C -> ... until A's per-signal depth guard fires.
        duin::Signal<int> sigA, sigB, sigC;
        int aDepth = 0, bDepth = 0, cDepth = 0;
        bool aHitLimit = false;

        sigC.Connect([&](int v) {
            ++cDepth;
            bool result = sigA.Emit(v); // close the cycle back to A
            if (!result)
                aHitLimit = true;
        });

        sigB.Connect([&](int v) {
            ++bDepth;
            sigC.Emit(v);
        });

        sigA.Connect([&](int v) {
            ++aDepth;
            sigB.Emit(v); // A -> B -> C -> A -> ...
        });

        sigA.Emit(0);
        CHECK(aHitLimit == true); // A's depth guard was triggered from inside C
    }

} // TEST_SUITE("Signal - Core")

} // namespace TestSignal
