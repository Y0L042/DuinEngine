#include <doctest.h>
#include <Duin/ECS/DECS/World.h>
#include <Duin/ECS/DECS/Entity.h>
#include <Duin/ECS/ECSComponents.h>
#include <Duin/ECS/Transform.h>
#include <Duin/Core/Maths/DuinMaths.h>
#include <cmath>

namespace TestTransform
{
using duin::Quaternion;
using duin::Vector3;
using duin::ECSComponent::Transform3D;

static bool Near(float a, float b, float eps = 1e-4f)
{
    return std::fabs(a - b) <= eps;
}

static bool Near(Vector3 a, Vector3 b, float eps = 1e-4f)
{
    return Near(a.x, b.x, eps) && Near(a.y, b.y, eps) && Near(a.z, b.z, eps);
}

static bool Near(Quaternion a, Quaternion b, float eps = 1e-4f)
{
    bool same = Near(a.x, b.x, eps) && Near(a.y, b.y, eps) && Near(a.z, b.z, eps) && Near(a.w, b.w, eps);
    bool negated = Near(a.x, -b.x, eps) && Near(a.y, -b.y, eps) && Near(a.z, -b.z, eps) && Near(a.w, -b.w, eps);
    return same || negated;
}

static Quaternion YawQuarter()
{
    return duin::QuaternionFromAxisAngle(Vector3(0.0f, 1.0f, 0.0f), 3.14159265f * 0.5f);
}

TEST_SUITE("Transform - Global")
{
    TEST_CASE("Root entity global values equal local values")
    {
        duin::World w;
        duin::Entity e = w.Entity("Root");
        e.Set<Transform3D>(Transform3D(Vector3(1.0f, 2.0f, 3.0f), Vector3(2.0f, 2.0f, 2.0f), YawQuarter()));

        CHECK(Near(duin::transform::GetGlobalPosition(e), Vector3(1.0f, 2.0f, 3.0f)));
        CHECK(Near(duin::transform::GetGlobalScale(e), Vector3(2.0f, 2.0f, 2.0f)));
        CHECK(Near(duin::transform::GetGlobalRotation(e), YawQuarter()));

        Transform3D g = duin::transform::GetGlobalTransform(e);
        CHECK(Near(g.GetPosition(), Vector3(1.0f, 2.0f, 3.0f)));
        CHECK(Near(g.GetScale(), Vector3(2.0f, 2.0f, 2.0f)));
        CHECK(Near(g.GetRotation(), YawQuarter()));
    }

    TEST_CASE("Child global position applies parent translation, scale and rotation")
    {
        duin::World w;
        duin::Entity parent = w.Entity("Parent");
        parent.Set<Transform3D>(Transform3D(Vector3(10.0f, 0.0f, 0.0f), Vector3(2.0f, 2.0f, 2.0f), YawQuarter()));

        duin::Entity child = w.Entity("Child");
        child.ChildOf(parent);
        child.Set<Transform3D>(Transform3D(Vector3(1.0f, 0.0f, 0.0f)));

        Vector3 expected = duin::Vector3Add(
            Vector3(10.0f, 0.0f, 0.0f), duin::Vector3RotateByQuaternion(Vector3(2.0f, 0.0f, 0.0f), YawQuarter()));

        CHECK(Near(duin::transform::GetGlobalPosition(child), expected));
        CHECK(Near(duin::transform::GetGlobalScale(child), Vector3(2.0f, 2.0f, 2.0f)));
        CHECK(Near(duin::transform::GetGlobalRotation(child), YawQuarter()));
        CHECK(Near(duin::transform::GetGlobalTransform(child).GetPosition(), expected));
    }

    TEST_CASE("Three-level chain with non-uniform scale composes")
    {
        duin::World w;
        duin::Entity a = w.Entity("A");
        a.Set<Transform3D>(Transform3D(Vector3(0.0f, 1.0f, 0.0f), Vector3(1.0f, 2.0f, 3.0f)));

        duin::Entity b = w.Entity("B");
        b.ChildOf(a);
        b.Set<Transform3D>(Transform3D(Vector3(0.0f, 1.0f, 1.0f), Vector3(2.0f, 1.0f, 1.0f)));

        duin::Entity c = w.Entity("C");
        c.ChildOf(b);
        c.Set<Transform3D>(Transform3D(Vector3(1.0f, 1.0f, 1.0f)));

        CHECK(Near(duin::transform::GetGlobalScale(c), Vector3(2.0f, 2.0f, 3.0f)));
        CHECK(Near(duin::transform::GetGlobalPosition(b), Vector3(0.0f, 3.0f, 3.0f)));
        CHECK(Near(duin::transform::GetGlobalPosition(c), Vector3(2.0f, 5.0f, 6.0f)));
        CHECK(Near(duin::transform::GetGlobalTransform(c).GetPosition(), Vector3(2.0f, 5.0f, 6.0f)));
    }

    TEST_CASE("Entity without Transform3D returns defaults")
    {
        duin::World w;
        duin::Entity e = w.Entity("Bare");

        CHECK(Near(duin::transform::GetGlobalPosition(e), Vector3(0.0f, 0.0f, 0.0f)));
        Transform3D g = duin::transform::GetGlobalTransform(e);
        CHECK(Near(g.GetPosition(), Vector3(0.0f, 0.0f, 0.0f)));
        CHECK(Near(g.GetScale(), Vector3(1.0f, 1.0f, 1.0f)));
        CHECK(Near(g.GetRotation(), duin::QuaternionIdentity()));

        duin::transform::SetGlobalPosition(e, Vector3(5.0f, 5.0f, 5.0f));
        CHECK_FALSE(e.Has<Transform3D>());
    }

    TEST_CASE("SetGlobalPosition on a child produces the matching local position")
    {
        duin::World w;
        duin::Entity parent = w.Entity("Parent");
        parent.Set<Transform3D>(Transform3D(Vector3(10.0f, 0.0f, 0.0f), Vector3(2.0f, 2.0f, 2.0f)));

        duin::Entity child = w.Entity("Child");
        child.ChildOf(parent);
        child.Set<Transform3D>(Transform3D());

        duin::transform::SetGlobalPosition(child, Vector3(14.0f, 2.0f, 0.0f));

        CHECK(Near(child.TryGet<Transform3D>()->GetPosition(), Vector3(2.0f, 1.0f, 0.0f)));
        CHECK(Near(duin::transform::GetGlobalPosition(child), Vector3(14.0f, 2.0f, 0.0f)));
    }

    TEST_CASE("SetGlobalTransform on a child produces matching local values")
    {
        duin::World w;
        duin::Entity parent = w.Entity("Parent");
        parent.Set<Transform3D>(Transform3D(Vector3(1.0f, 0.0f, 0.0f), Vector3(2.0f, 2.0f, 2.0f), YawQuarter()));

        duin::Entity child = w.Entity("Child");
        child.ChildOf(parent);
        child.Set<Transform3D>(Transform3D());

        Transform3D target(Vector3(3.0f, 4.0f, 5.0f), Vector3(4.0f, 4.0f, 4.0f), YawQuarter());
        duin::transform::SetGlobalTransform(child, target);

        const Transform3D *local = child.TryGet<Transform3D>();
        REQUIRE(local != nullptr);
        CHECK(Near(local->GetScale(), Vector3(2.0f, 2.0f, 2.0f)));
        CHECK(Near(local->GetRotation(), duin::QuaternionIdentity()));

        Transform3D g = duin::transform::GetGlobalTransform(child);
        CHECK(Near(g.GetPosition(), target.GetPosition()));
        CHECK(Near(g.GetScale(), target.GetScale()));
        CHECK(Near(g.GetRotation(), target.GetRotation()));
    }

    TEST_CASE("SetGlobalScale and SetGlobalRotation on a child round-trip")
    {
        duin::World w;
        duin::Entity parent = w.Entity("Parent");
        parent.Set<Transform3D>(Transform3D(Vector3(0.0f, 0.0f, 0.0f), Vector3(2.0f, 4.0f, 8.0f), YawQuarter()));

        duin::Entity child = w.Entity("Child");
        child.ChildOf(parent);
        child.Set<Transform3D>(Transform3D());

        duin::transform::SetGlobalScale(child, Vector3(4.0f, 4.0f, 4.0f));
        duin::transform::SetGlobalRotation(child, duin::QuaternionIdentity());

        CHECK(Near(duin::transform::GetGlobalTransform(child).GetScale(), Vector3(4.0f, 4.0f, 4.0f)));
        CHECK(Near(duin::transform::GetGlobalTransform(child).GetRotation(), duin::QuaternionIdentity()));
    }

    TEST_CASE("Child global position follows parent after parent moves" * doctest::may_fail())
    {
        duin::World w;
        duin::Entity parent = w.Entity("Parent");
        parent.Set<Transform3D>(Transform3D(Vector3(0.0f, 0.0f, 0.0f)));

        duin::Entity child = w.Entity("Child");
        child.ChildOf(parent);
        child.Set<Transform3D>(Transform3D(Vector3(1.0f, 0.0f, 0.0f)));

        CHECK(Near(duin::transform::GetGlobalPosition(child), Vector3(1.0f, 0.0f, 0.0f)));

        parent.TryGetMut<Transform3D>()->SetPosition(Vector3(10.0f, 0.0f, 0.0f));

        CHECK(Near(duin::transform::GetGlobalPosition(child), Vector3(11.0f, 0.0f, 0.0f)));
    }

    TEST_CASE("Child global scale and rotation follow parent after parent changes" * doctest::may_fail())
    {
        duin::World w;
        duin::Entity parent = w.Entity("Parent");
        parent.Set<Transform3D>(Transform3D());

        duin::Entity child = w.Entity("Child");
        child.ChildOf(parent);
        child.Set<Transform3D>(Transform3D());

        CHECK(Near(duin::transform::GetGlobalScale(child), Vector3(1.0f, 1.0f, 1.0f)));
        CHECK(Near(duin::transform::GetGlobalRotation(child), duin::QuaternionIdentity()));

        parent.TryGetMut<Transform3D>()->SetScale(Vector3(3.0f, 3.0f, 3.0f));
        parent.TryGetMut<Transform3D>()->SetRotation(YawQuarter());

        CHECK(Near(duin::transform::GetGlobalScale(child), Vector3(3.0f, 3.0f, 3.0f)));
        CHECK(Near(duin::transform::GetGlobalRotation(child), YawQuarter()));
    }
}

TEST_SUITE("Transform - Transform3D cache")
{
    TEST_CASE("Local setters mark all global caches dirty")
    {
        Transform3D tx;
        tx._UpdateGlobalPositionCache(Vector3(1.0f, 1.0f, 1.0f));
        tx._UpdateGlobalScaleCache(Vector3(1.0f, 1.0f, 1.0f));
        tx._UpdateGlobalRotationCache(duin::QuaternionIdentity());
        REQUIRE_FALSE(tx._GetGlobalPositionCacheDirtyFlag());
        REQUIRE_FALSE(tx._GetGlobalScaleCacheDirtyFlag());
        REQUIRE_FALSE(tx._GetGlobalRotationCacheDirtyFlag());

        SUBCASE("SetPosition")
        {
            tx.SetPosition(Vector3(2.0f, 0.0f, 0.0f));
        }
        SUBCASE("SetScale")
        {
            tx.SetScale(Vector3(2.0f, 2.0f, 2.0f));
        }
        SUBCASE("SetRotation")
        {
            tx.SetRotation(YawQuarter());
        }

        CHECK(tx._GetGlobalPositionCacheDirtyFlag());
        CHECK(tx._GetGlobalScaleCacheDirtyFlag());
        CHECK(tx._GetGlobalRotationCacheDirtyFlag());
    }

    TEST_CASE("Cache update and get round-trip clears dirty flags")
    {
        Transform3D tx;
        tx._UpdateGlobalPositionCache(Vector3(4.0f, 5.0f, 6.0f));
        tx._UpdateGlobalScaleCache(Vector3(7.0f, 8.0f, 9.0f));
        tx._UpdateGlobalRotationCache(YawQuarter());

        CHECK(Near(tx._GetGlobalPositionCache(), Vector3(4.0f, 5.0f, 6.0f)));
        CHECK(Near(tx._GetGlobalScaleCache(), Vector3(7.0f, 8.0f, 9.0f)));
        CHECK(Near(tx._GetGlobalRotationCache(), YawQuarter()));
        CHECK_FALSE(tx._GetGlobalPositionCacheDirtyFlag());
        CHECK_FALSE(tx._GetGlobalScaleCacheDirtyFlag());
        CHECK_FALSE(tx._GetGlobalRotationCacheDirtyFlag());
    }

    TEST_CASE("Dirty flag setters toggle each flag independently")
    {
        Transform3D tx;
        tx._SetGlobalPositionCacheDirtyFlag(false);
        tx._SetGlobalScaleCacheDirtyFlag(true);
        tx._SetGlobalRotationCacheDirtyFlag(false);

        CHECK_FALSE(tx._GetGlobalPositionCacheDirtyFlag());
        CHECK(tx._GetGlobalScaleCacheDirtyFlag());
        CHECK_FALSE(tx._GetGlobalRotationCacheDirtyFlag());
    }

    TEST_CASE("GetGlobalTransform refreshes the cache of a root entity")
    {
        duin::World w;
        duin::Entity e = w.Entity("Root");
        e.Set<Transform3D>(Transform3D(Vector3(3.0f, 2.0f, 1.0f)));

        duin::transform::GetGlobalTransform(e);

        const Transform3D *tx = e.TryGet<Transform3D>();
        REQUIRE(tx != nullptr);
        CHECK_FALSE(tx->_GetGlobalPositionCacheDirtyFlag());
        CHECK(Near(const_cast<Transform3D *>(tx)->_GetGlobalPositionCache(), Vector3(3.0f, 2.0f, 1.0f)));
    }
}
} // namespace TestTransform
