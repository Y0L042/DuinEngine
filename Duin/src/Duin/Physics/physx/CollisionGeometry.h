#pragma once

#include "Duin/Core/Maths/DuinMaths.h"

#include <PxPhysicsAPI.h>
#include <rfl.hpp>

namespace duin
{

enum CollisionGeometryType
{
    cBOX = 0,
    cSPHERE,
    cCAPSULE,
    cPLANE,
    cCONVEXMESH,
    cTRIANGLEMESH,
    cCUSTOM,
    cNULL
};

class CollisionGeometry
{
  public:
    CollisionGeometryType type = cNULL;
};

class BoxGeometry : public CollisionGeometry
{
  public:
    float width, height, depth;

    BoxGeometry(float width, float height, float depth);
    BoxGeometry(Vector3 size);

    using ReflectionType = struct
    {
        float w, h, d;
    };
    BoxGeometry(const ReflectionType &impl) : BoxGeometry(impl.w, impl.h, impl.d)
    {
    }
    const ReflectionType &reflection() const
    {
        return ReflectionType{width, height, depth};
    }

  private:
    friend class CollisionShape;
    friend class StaticBody;

    physx::PxBoxGeometry pxBoxGeometry;
};

class SphereGeometry : public CollisionGeometry
{
  public:
    float radius;

    SphereGeometry(float radius);

    using ReflectionType = struct
    {
        float v;
    };
    SphereGeometry(const ReflectionType &impl) : SphereGeometry(impl.v)
    {
    }
    const ReflectionType &reflection() const
    {
        return ReflectionType{radius};
    }

  private:
    friend class CollisionShape;
    friend class StaticBody;

    physx::PxSphereGeometry pxSphereGeometry;
};

class CapsuleGeometry : public CollisionGeometry
{
  public:
    float radius, height;

    CapsuleGeometry(float radius, float height);

    using ReflectionType = struct
    {
        float r, h;
    };
    CapsuleGeometry(const ReflectionType &impl) : CapsuleGeometry(impl.r, impl.h)
    {
    }
    const ReflectionType &reflection() const
    {
        return ReflectionType{radius, height};
    }

  private:
    friend class CollisionShape;

    physx::PxCapsuleGeometry pxCapsuleGeometry;
};

class PlaneGeometry : public CollisionGeometry
{
  public:
    PlaneGeometry();

    using ReflectionType = struct
    {
    };
    PlaneGeometry(const ReflectionType &impl) : PlaneGeometry()
    {
    }
    const ReflectionType &reflection() const
    {
        return ReflectionType{};
    }

  private:
    friend class CollisionShape;
    friend class StaticBody;

    physx::PxPlaneGeometry pxPlaneGeometry;
};

// TODO
class ConvexMeshGeometry : public CollisionGeometry
{
  public:
    ConvexMeshGeometry();

    using ReflectionType = struct
    {
        float v;
    };
    ConvexMeshGeometry(const ReflectionType &impl) : ConvexMeshGeometry()
    {
    }
    const ReflectionType &reflection() const
    {
        return ReflectionType{};
    }
};

// TODO
class TriangleMeshGeometry : public CollisionGeometry
{
  public:
    TriangleMeshGeometry();

    using ReflectionType = struct
    {
        float v;
    };
    TriangleMeshGeometry(const ReflectionType &impl) : TriangleMeshGeometry()
    {
    }
    const ReflectionType &reflection() const
    {
        return ReflectionType{};
    }
};

} // namespace duin
