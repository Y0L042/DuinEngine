#include <doctest.h>
#include <Duin/Core/Events/EventsModule.h>

namespace TestInputDevices
{

// ============================================================================
// InputDevices singletons
// ============================================================================

TEST_SUITE("InputDevices")
{
    TEST_CASE("GetKeyboard_01 - returns non-null shared_ptr")
    {
        auto kb = duin::GetKeyboard_01();
        CHECK(kb != nullptr);
    }

    TEST_CASE("GetKeyboard_01 - two calls return the same instance")
    {
        auto kb1 = duin::GetKeyboard_01();
        auto kb2 = duin::GetKeyboard_01();
        CHECK(kb1 != nullptr);
        CHECK(kb2 != nullptr);
        CHECK(kb1->uuid == kb2->uuid);
    }

    TEST_CASE("GetMouse_01 - returns non-null shared_ptr")
    {
        auto mouse = duin::GetMouse_01();
        CHECK(mouse != nullptr);
    }

    TEST_CASE("GetMouse_01 - two calls return the same instance")
    {
        auto m1 = duin::GetMouse_01();
        auto m2 = duin::GetMouse_01();
        CHECK(m1 != nullptr);
        CHECK(m2 != nullptr);
        CHECK(m1->uuid == m2->uuid);
    }

    TEST_CASE("GetKeyboard_02 - returns non-null shared_ptr")
    {
        auto kb = duin::GetKeyboard_02();
        CHECK(kb != nullptr);
    }

    TEST_CASE("GetKeyboard_02 - distinct from GetKeyboard_01")
    {
        auto kb1 = duin::GetKeyboard_01();
        auto kb2 = duin::GetKeyboard_02();
        CHECK(kb1 != nullptr);
        CHECK(kb2 != nullptr);
        CHECK_FALSE(kb1->uuid == kb2->uuid);
    }

    TEST_CASE("GetMouse_02 - returns non-null shared_ptr")
    {
        auto mouse = duin::GetMouse_02();
        CHECK(mouse != nullptr);
    }

    TEST_CASE("GetMouse_02 - distinct from GetMouse_01")
    {
        auto m1 = duin::GetMouse_01();
        auto m2 = duin::GetMouse_02();
        CHECK(m1 != nullptr);
        CHECK(m2 != nullptr);
        CHECK_FALSE(m1->uuid == m2->uuid);
    }
}

} // namespace TestInputDevices
