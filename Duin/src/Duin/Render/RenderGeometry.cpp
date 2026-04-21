#include "dnpch.h"
#include "RenderGeometry.h"

namespace duin
{
BoxRenderGeometry::BoxRenderGeometry() : RenderGeometry(RenderGeometryType::BOX)
{
}

BoxRenderGeometry::BoxRenderGeometry(float width, float height, float depth)
    : RenderGeometry(RenderGeometryType::BOX), width(width), height(height), depth(depth)
{
}

PosColorVertex *BoxRenderGeometry::GetIdentityVertices()
{
    static PosColorVertex vertices[VERT_SIZE] = {{-1.0f, 1.0f, 1.0f, 0xff000000},   {1.0f, 1.0f, 1.0f, 0xff0000ff},
                                                 {-1.0f, -1.0f, 1.0f, 0xff00ff00},  {1.0f, -1.0f, 1.0f, 0xff00ffff},
                                                 {-1.0f, 1.0f, -1.0f, 0xffff0000},  {1.0f, 1.0f, -1.0f, 0xffff00ff},
                                                 {-1.0f, -1.0f, -1.0f, 0xffffff00}, {1.0f, -1.0f, -1.0f, 0xffffffff}};

    return vertices;
}

uint16_t *BoxRenderGeometry::GetIdentityTriList()
{
    static uint16_t triList[TRI_SIZE] = {
        0, 1, 2, // Triangle 1
        1, 3, 2, // Triangle 2
        4, 6, 5, // Triangle 3
        5, 6, 7, // Triangle 4
        0, 2, 4, // Triangle 5
        4, 2, 6, // Triangle 6
        1, 5, 3, // Triangle 7
        5, 7, 3, // Triangle 8
        0, 4, 1, // Triangle 9
        4, 5, 1, // Triangle 10
        2, 3, 6, // Triangle 11
        6, 3, 7  // Triangle 12
    };

    return triList;
}

SphereRenderGeometry::SphereRenderGeometry() : RenderGeometry(RenderGeometryType::SPHERE)
{
}

SphereRenderGeometry::SphereRenderGeometry(float radius) : RenderGeometry(RenderGeometryType::SPHERE), radius(radius)
{
}

PosColorVertex *SphereRenderGeometry::GetIdentityVertices()
{
    static PosColorVertex vertices[VERT_SIZE] = {{-1.0f, 1.0f, 1.0f, 0xff000000},   {1.0f, 1.0f, 1.0f, 0xff0000ff},
                                                 {-1.0f, -1.0f, 1.0f, 0xff00ff00},  {1.0f, -1.0f, 1.0f, 0xff00ffff},
                                                 {-1.0f, 1.0f, -1.0f, 0xffff0000},  {1.0f, 1.0f, -1.0f, 0xffff00ff},
                                                 {-1.0f, -1.0f, -1.0f, 0xffffff00}, {1.0f, -1.0f, -1.0f, 0xffffffff}};

    return vertices;
}

uint16_t *SphereRenderGeometry::GetIdentityTriList()
{
    static uint16_t triList[TRI_SIZE] = {
        0, 1, 2, // Triangle 1
        1, 3, 2, // Triangle 2
        4, 6, 5, // Triangle 3
        5, 6, 7, // Triangle 4
        0, 2, 4, // Triangle 5
        4, 2, 6, // Triangle 6
        1, 5, 3, // Triangle 7
        5, 7, 3, // Triangle 8
        0, 4, 1, // Triangle 9
        4, 5, 1, // Triangle 10
        2, 3, 6, // Triangle 11
        6, 3, 7  // Triangle 12
    };

    return triList;
}

CapsuleRenderGeometry::CapsuleRenderGeometry() : RenderGeometry(RenderGeometryType::CAPSULE)
{
}

CapsuleRenderGeometry::CapsuleRenderGeometry(float radius, float height)
    : RenderGeometry(RenderGeometryType::CAPSULE), radius(radius), height(height)
{
}

// Unit capsule: radius=1, halfCylinderHeight=1, total height=4.
// Topology: top pole → top ring → bottom ring → bottom pole.
// 8 radial segments; caps are cone fans from the poles.
PosColorVertex *CapsuleRenderGeometry::GetIdentityVertices()
{
    static PosColorVertex vertices[VERT_SIZE] = {
        // [0] top pole
        { 0.0f,  2.0f,  0.0f, 0xffffffff },
        // [1-8] top cylinder ring (y=+1, r=1)
        { 1.000f,  1.0f,  0.000f, 0xffffffff },
        { 0.707f,  1.0f,  0.707f, 0xffffffff },
        { 0.000f,  1.0f,  1.000f, 0xffffffff },
        {-0.707f,  1.0f,  0.707f, 0xffffffff },
        {-1.000f,  1.0f,  0.000f, 0xffffffff },
        {-0.707f,  1.0f, -0.707f, 0xffffffff },
        { 0.000f,  1.0f, -1.000f, 0xffffffff },
        { 0.707f,  1.0f, -0.707f, 0xffffffff },
        // [9-16] bottom cylinder ring (y=-1, r=1)
        { 1.000f, -1.0f,  0.000f, 0xffffffff },
        { 0.707f, -1.0f,  0.707f, 0xffffffff },
        { 0.000f, -1.0f,  1.000f, 0xffffffff },
        {-0.707f, -1.0f,  0.707f, 0xffffffff },
        {-1.000f, -1.0f,  0.000f, 0xffffffff },
        {-0.707f, -1.0f, -0.707f, 0xffffffff },
        { 0.000f, -1.0f, -1.000f, 0xffffffff },
        { 0.707f, -1.0f, -0.707f, 0xffffffff },
        // [17] bottom pole
        { 0.0f, -2.0f,  0.0f, 0xffffffff },
    };
    return vertices;
}

uint16_t *CapsuleRenderGeometry::GetIdentityTriList()
{
    static uint16_t triList[TRI_SIZE] = {
        // Top cap fan from pole (v0) to top ring (v1..v8)
        0, 1, 2,   0, 2, 3,   0, 3, 4,   0, 4, 5,
        0, 5, 6,   0, 6, 7,   0, 7, 8,   0, 8, 1,
        // Side quads (top ring → bottom ring)
        1,  9,  2,   2,  9, 10,
        2, 10,  3,   3, 10, 11,
        3, 11,  4,   4, 11, 12,
        4, 12,  5,   5, 12, 13,
        5, 13,  6,   6, 13, 14,
        6, 14,  7,   7, 14, 15,
        7, 15,  8,   8, 15, 16,
        8, 16,  1,   1, 16,  9,
        // Bottom cap fan from bottom ring (v9..v16) to pole (v17) — reversed winding
        17, 10,  9,  17, 11, 10,  17, 12, 11,  17, 13, 12,
        17, 14, 13,  17, 15, 14,  17, 16, 15,  17,  9, 16,
    };
    return triList;
}

PlaneRenderGeometry::PlaneRenderGeometry() : RenderGeometry(RenderGeometryType::PLANE)
{
}

PosColorVertex *PlaneRenderGeometry::GetIdentityVertices()
{
    static PosColorVertex vertices[VERT_SIZE] = {{-1.0f, 0.0f, -1.0f, 0xff000000},
                                                 {1.0f, 0.0f, -1.0f, 0xff0000ff},
                                                 {-1.0f, 0.0f, 1.0f, 0xff00ff00},
                                                 {1.0f, 0.0f, 1.0f, 0xff00ffff}};

    return vertices;
}

uint16_t *PlaneRenderGeometry::GetIdentityTriList()
{
    static uint16_t triList[TRI_SIZE] = {
        0, 1, 2, // Triangle 1
        1, 3, 2  // Triangle 2
    };

    return triList;
}

ConeRenderGeometry::ConeRenderGeometry() : RenderGeometry(RenderGeometryType::CONE)
{
}

ConeRenderGeometry::ConeRenderGeometry(float radius, float height)
    : RenderGeometry(RenderGeometryType::CONE), radius(radius), height(height)
{
}

// Unit cone: base at y=-1 (radius=1), tip at y=+1.  8 segments.
PosColorVertex *ConeRenderGeometry::GetIdentityVertices()
{
    static PosColorVertex vertices[VERT_SIZE] = {
        // [0-7] base ring (y=-1)
        { 1.000f, -1.0f,  0.000f, 0xffffffff },
        { 0.707f, -1.0f,  0.707f, 0xffffffff },
        { 0.000f, -1.0f,  1.000f, 0xffffffff },
        {-0.707f, -1.0f,  0.707f, 0xffffffff },
        {-1.000f, -1.0f,  0.000f, 0xffffffff },
        {-0.707f, -1.0f, -0.707f, 0xffffffff },
        { 0.000f, -1.0f, -1.000f, 0xffffffff },
        { 0.707f, -1.0f, -0.707f, 0xffffffff },
        // [8] base center
        { 0.0f,  -1.0f,  0.0f,   0xffffffff },
        // [9] tip
        { 0.0f,   1.0f,  0.0f,   0xffffffff },
    };
    return vertices;
}

uint16_t *ConeRenderGeometry::GetIdentityTriList()
{
    static uint16_t triList[TRI_SIZE] = {
        // Side: fan from tip (v9) to base ring
        9, 0, 1,   9, 1, 2,   9, 2, 3,   9, 3, 4,
        9, 4, 5,   9, 5, 6,   9, 6, 7,   9, 7, 0,
        // Base cap: fan from center (v8) — reversed winding for outward normal
        8, 1, 0,   8, 2, 1,   8, 3, 2,   8, 4, 3,
        8, 5, 4,   8, 6, 5,   8, 7, 6,   8, 0, 7,
    };
    return triList;
}

CylinderRenderGeometry::CylinderRenderGeometry() : RenderGeometry(RenderGeometryType::CYLINDER)
{
}

CylinderRenderGeometry::CylinderRenderGeometry(float radius, float height)
    : RenderGeometry(RenderGeometryType::CYLINDER), radius(radius), height(height)
{
}

// Unit cylinder: radius=1, halfHeight=1.  8 segments.
PosColorVertex *CylinderRenderGeometry::GetIdentityVertices()
{
    static PosColorVertex vertices[VERT_SIZE] = {
        // [0-7] top ring (y=+1)
        { 1.000f,  1.0f,  0.000f, 0xffffffff },
        { 0.707f,  1.0f,  0.707f, 0xffffffff },
        { 0.000f,  1.0f,  1.000f, 0xffffffff },
        {-0.707f,  1.0f,  0.707f, 0xffffffff },
        {-1.000f,  1.0f,  0.000f, 0xffffffff },
        {-0.707f,  1.0f, -0.707f, 0xffffffff },
        { 0.000f,  1.0f, -1.000f, 0xffffffff },
        { 0.707f,  1.0f, -0.707f, 0xffffffff },
        // [8-15] bottom ring (y=-1)
        { 1.000f, -1.0f,  0.000f, 0xffffffff },
        { 0.707f, -1.0f,  0.707f, 0xffffffff },
        { 0.000f, -1.0f,  1.000f, 0xffffffff },
        {-0.707f, -1.0f,  0.707f, 0xffffffff },
        {-1.000f, -1.0f,  0.000f, 0xffffffff },
        {-0.707f, -1.0f, -0.707f, 0xffffffff },
        { 0.000f, -1.0f, -1.000f, 0xffffffff },
        { 0.707f, -1.0f, -0.707f, 0xffffffff },
        // [16] top center, [17] bottom center
        { 0.0f,   1.0f,  0.0f,   0xffffffff },
        { 0.0f,  -1.0f,  0.0f,   0xffffffff },
    };
    return vertices;
}

uint16_t *CylinderRenderGeometry::GetIdentityTriList()
{
    static uint16_t triList[TRI_SIZE] = {
        // Top cap fan from v16
        16, 0, 1,   16, 1, 2,   16, 2, 3,   16, 3, 4,
        16, 4, 5,   16, 5, 6,   16, 6, 7,   16, 7, 0,
        // Bottom cap fan from v17 — reversed winding
        17,  9,  8,  17, 10,  9,  17, 11, 10,  17, 12, 11,
        17, 13, 12,  17, 14, 13,  17, 15, 14,  17,  8, 15,
        // Side quads
         0,  8,  1,   1,  8,  9,
         1,  9,  2,   2,  9, 10,
         2, 10,  3,   3, 10, 11,
         3, 11,  4,   4, 11, 12,
         4, 12,  5,   5, 12, 13,
         5, 13,  6,   6, 13, 14,
         6, 14,  7,   7, 14, 15,
         7, 15,  0,   0, 15,  8,
    };
    return triList;
}

DiskRenderGeometry::DiskRenderGeometry() : RenderGeometry(RenderGeometryType::DISK)
{
}

DiskRenderGeometry::DiskRenderGeometry(float radius) : RenderGeometry(RenderGeometryType::DISK), radius(radius)
{
}

// Unit disk: radius=1, flat in XZ plane (y=0).  8 segments, one-sided.
PosColorVertex *DiskRenderGeometry::GetIdentityVertices()
{
    static PosColorVertex vertices[VERT_SIZE] = {
        // [0-7] edge ring (y=0)
        { 1.000f, 0.0f,  0.000f, 0xffffffff },
        { 0.707f, 0.0f,  0.707f, 0xffffffff },
        { 0.000f, 0.0f,  1.000f, 0xffffffff },
        {-0.707f, 0.0f,  0.707f, 0xffffffff },
        {-1.000f, 0.0f,  0.000f, 0xffffffff },
        {-0.707f, 0.0f, -0.707f, 0xffffffff },
        { 0.000f, 0.0f, -1.000f, 0xffffffff },
        { 0.707f, 0.0f, -0.707f, 0xffffffff },
        // [8] center
        { 0.0f,   0.0f,  0.0f,   0xffffffff },
    };
    return vertices;
}

uint16_t *DiskRenderGeometry::GetIdentityTriList()
{
    static uint16_t triList[TRI_SIZE] = {
        8, 0, 1,   8, 1, 2,   8, 2, 3,   8, 3, 4,
        8, 4, 5,   8, 5, 6,   8, 6, 7,   8, 7, 0,
    };
    return triList;
}

TriangleRenderGeometry::TriangleRenderGeometry() : RenderGeometry(RenderGeometryType::TRIANGLE)
{
}

TriangleRenderGeometry::TriangleRenderGeometry(float a, float b, float c)
    : RenderGeometry(RenderGeometryType::TRIANGLE), a(a), b(b), c(c)
{
}

// Unit equilateral triangle inscribed in unit circle, flat in XZ plane (y=0).  Two-sided.
PosColorVertex *TriangleRenderGeometry::GetIdentityVertices()
{
    static PosColorVertex vertices[VERT_SIZE] = {
        { 0.000f,  0.0f,  1.000f, 0xffffffff },   // v0: front
        { 0.866f,  0.0f, -0.500f, 0xffffffff },   // v1: back-right
        {-0.866f,  0.0f, -0.500f, 0xffffffff },   // v2: back-left
    };
    return vertices;
}

uint16_t *TriangleRenderGeometry::GetIdentityTriList()
{
    static uint16_t triList[TRI_SIZE] = {
        0, 1, 2,   // front face
        0, 2, 1,   // back face
    };
    return triList;
}

TriangleMeshRenderGeometry::TriangleMeshRenderGeometry() : RenderGeometry(RenderGeometryType::TRIANGLEMESH)
{
}
} // namespace duin
