#include <doctest.h>
#include <Duin/Script/ScriptMemory.h>
#include <memory>

namespace TestScript
{
TEST_SUITE("ScriptMemory")
{
    TEST_CASE("Add returns the raw pointer as key")
    {
        duin::ScriptMemory mem;
        auto obj = std::make_shared<int>(42);
        void *expected = obj.get();

        duin::ScriptMemory::Key key = mem.Add(obj);

        CHECK(key == expected);
    }

    TEST_CASE("Add keeps object alive after caller drops shared_ptr")
    {
        duin::ScriptMemory mem;
        std::weak_ptr<int> weak;

        int *handle;
        {
            auto obj = std::make_shared<int>(99);
            weak = obj;
            handle = mem.Add(obj);
        }

        // caller's shared_ptr is gone; ScriptMemory must still hold it
        CHECK(!weak.expired());
        CHECK(*handle == 99);
    }

    TEST_CASE("Remove releases the object when no external references exist")
    {
        duin::ScriptMemory mem;
        std::weak_ptr<int> weak;

        auto obj = std::make_shared<int>(7);
        weak = obj;
        duin::ScriptMemory::Key key = mem.Add(obj);
        obj.reset();

        CHECK(!weak.expired()); // still alive inside memory

        mem.Remove(key);

        CHECK(weak.expired()); // now released
    }

    TEST_CASE("Remove does not affect other entries")
    {
        duin::ScriptMemory mem;
        std::weak_ptr<int> weakA, weakB;

        auto a = std::make_shared<int>(1);
        auto b = std::make_shared<int>(2);
        weakA = a;
        weakB = b;

        auto keyA = mem.Add(a);
        mem.Add(b);
        a.reset();
        b.reset();

        mem.Remove(keyA);

        CHECK(weakA.expired());
        CHECK(!weakB.expired());
    }

    TEST_CASE("ClearMemory releases all held objects")
    {
        duin::ScriptMemory mem;
        std::weak_ptr<int> weakA, weakB, weakC;

        auto a = std::make_shared<int>(1);
        auto b = std::make_shared<int>(2);
        auto c = std::make_shared<int>(3);
        weakA = a;
        weakB = b;
        weakC = c;

        mem.Add(a);
        mem.Add(b);
        mem.Add(c);
        a.reset();
        b.reset();
        c.reset();

        CHECK(!weakA.expired());
        CHECK(!weakB.expired());
        CHECK(!weakC.expired());

        mem.ClearMemory();

        CHECK(weakA.expired());
        CHECK(weakB.expired());
        CHECK(weakC.expired());
    }

    TEST_CASE("Remove on unknown key is a no-op")
    {
        duin::ScriptMemory mem;
        int dummy = 0;
        void *garbage = &dummy;

        // Must not crash or throw
        CHECK_NOTHROW(mem.Remove(garbage));
    }

    TEST_CASE("ClearMemory on empty ScriptMemory is a no-op")
    {
        duin::ScriptMemory mem;
        CHECK_NOTHROW(mem.ClearMemory());
    }

    TEST_CASE("Add same object twice stores it once; Remove releases it")
    {
        duin::ScriptMemory mem;
        std::weak_ptr<int> weak;

        auto obj = std::make_shared<int>(5);
        weak = obj;

        auto key1 = mem.Add(obj);
        auto key2 = mem.Add(obj); // same raw ptr → overwrites the map entry

        CHECK(key1 == key2);

        obj.reset();
        CHECK(!weak.expired());

        mem.Remove(key1);
        CHECK(weak.expired());
    }

    TEST_CASE("Add works with multiple distinct types")
    {
        duin::ScriptMemory mem;
        std::weak_ptr<int> weakInt;
        std::weak_ptr<double> weakDouble;

        auto i = std::make_shared<int>(1);
        auto d = std::make_shared<double>(3.14);
        weakInt = i;
        weakDouble = d;

        auto keyI = mem.Add(i);
        auto keyD = mem.Add(d);
        i.reset();
        d.reset();

        CHECK(!weakInt.expired());
        CHECK(!weakDouble.expired());

        mem.Remove(keyI);
        mem.Remove(keyD);

        CHECK(weakInt.expired());
        CHECK(weakDouble.expired());
    }
}
} // namespace TestScript