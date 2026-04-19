#include "dnpch.h"
#include "CollisionGeometry.h"

duin::BoxGeometry::BoxGeometry(float width, float height, float depth)
    : width(width), height(height), depth(depth)
{
    type = cBOX;
}

duin::BoxGeometry::BoxGeometry(duin::Vector3 size)
    : BoxGeometry(size.x, size.y, size.z)
{
}

duin::SphereGeometry::SphereGeometry(float radius)
    : radius(radius)
{
    type = cSPHERE;
}

duin::CapsuleGeometry::CapsuleGeometry(float radius, float height)
    : radius(radius), height(height)
{
    type = cCAPSULE;
}

duin::PlaneGeometry::PlaneGeometry()
{
    type = cPLANE;
}

duin::ConvexMeshGeometry::ConvexMeshGeometry()
{
    type = cCONVEXMESH;
}

duin::TriangleMeshGeometry::TriangleMeshGeometry()
{
    type = cTRIANGLEMESH;
}
