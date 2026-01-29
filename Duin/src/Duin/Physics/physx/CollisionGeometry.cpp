#include "dnpch.h"
#include "CollisionGeometry.h"

namespace duin
{
BoxGeometry::BoxGeometry(float width, float height, float depth)
    : width(width), height(height), depth(depth), pxBoxGeometry(width / 2.0f, height / 2.0f, depth / 2.0f)
{
    type = cBOX;
}

BoxGeometry::BoxGeometry(Vector3 size)
    : width(size.x), height(size.y), depth(size.z), pxBoxGeometry(size.x / 2.0f, size.y / 2.0f, size.z / 2.0f)
{
    type = cBOX;
}

SphereGeometry::SphereGeometry(float radius) : radius(radius), pxSphereGeometry(radius)
{
    type = cSPHERE;
}

CapsuleGeometry::CapsuleGeometry(float radius, float height)
    : radius(radius), height(height), pxCapsuleGeometry(radius, height / 2.0f)
{
    type = cCAPSULE;
}

PlaneGeometry::PlaneGeometry() : pxPlaneGeometry()
{
    type = cPLANE;
}

ConvexMeshGeometry::ConvexMeshGeometry()
{
    type = cCONVEXMESH;
}

TriangleMeshGeometry::TriangleMeshGeometry()
{
    type = cTRIANGLEMESH;
}
} // namespace duin
