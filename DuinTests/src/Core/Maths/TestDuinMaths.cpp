#include <doctest.h>
#include <Duin/Core/Maths/DuinMaths.h>
#include <type_traits>

namespace TestDuinMaths
{
TEST_SUITE("Maths - Vector constructors")
{
    TEST_CASE("Vector2 broadcast constructor fills both components")
    {
        duin::Vector2 v(3.5f);
        CHECK(v.x == 3.5f);
        CHECK(v.y == 3.5f);
    }

    TEST_CASE("Vector3 broadcast constructor fills all components")
    {
        duin::Vector3 v(-2.0f);
        CHECK(v.x == -2.0f);
        CHECK(v.y == -2.0f);
        CHECK(v.z == -2.0f);
    }

    TEST_CASE("Default constructors are zero")
    {
        duin::Vector2 v2;
        duin::Vector3 v3;
        CHECK(v2.x == 0.0f);
        CHECK(v2.y == 0.0f);
        CHECK(v3.x == 0.0f);
        CHECK(v3.y == 0.0f);
        CHECK(v3.z == 0.0f);
    }

    TEST_CASE("Component constructors keep their order")
    {
        duin::Vector2 v2(1.0f, 2.0f);
        duin::Vector3 v3(1.0f, 2.0f, 3.0f);
        CHECK(v2.x == 1.0f);
        CHECK(v2.y == 2.0f);
        CHECK(v3.x == 1.0f);
        CHECK(v3.y == 2.0f);
        CHECK(v3.z == 3.0f);
    }

    TEST_CASE("A float converts implicitly to a broadcast vector")
    {
        static_assert(std::is_convertible_v<float, duin::Vector2>);
        static_assert(std::is_convertible_v<float, duin::Vector3>);

        duin::Vector3 scaled = duin::Vector3Multiply(duin::Vector3(1.0f, 2.0f, 3.0f), 2.0f);
        CHECK(scaled.x == 2.0f);
        CHECK(scaled.y == 4.0f);
        CHECK(scaled.z == 6.0f);
    }
}
} // namespace TestDuinMaths
