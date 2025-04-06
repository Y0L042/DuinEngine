#include "dnpch.h"
#include "RenderGeometry.h"

namespace duin {
	BoxRenderGeometry::BoxRenderGeometry()
		: RenderGeometry(RenderGeometryType::BOX)
	{}

	BoxRenderGeometry::BoxRenderGeometry(float width, float height, float depth)
		: RenderGeometry(RenderGeometryType::BOX), width(width), height(height), depth(depth)
	{}

    PosColorVertex *BoxRenderGeometry::GetIdentityVertices()
    {
        static PosColorVertex vertices[VERT_SIZE] = {
            { -1.0f,  1.0f,  1.0f, 0xff000000 },
            {  1.0f,  1.0f,  1.0f, 0xff0000ff },
            { -1.0f, -1.0f,  1.0f, 0xff00ff00 },
            {  1.0f, -1.0f,  1.0f, 0xff00ffff },
            { -1.0f,  1.0f, -1.0f, 0xffff0000 },
            {  1.0f,  1.0f, -1.0f, 0xffff00ff },
            { -1.0f, -1.0f, -1.0f, 0xffffff00 },
            {  1.0f, -1.0f, -1.0f, 0xffffffff }
        };

        return vertices;
    }

    uint16_t *BoxRenderGeometry::GetIdentityTriList()
    {
        static uint16_t triList[TRI_SIZE] = {
            0, 1, 2,  // Triangle 1
            1, 3, 2,  // Triangle 2
            4, 6, 5,  // Triangle 3
            5, 6, 7,  // Triangle 4
            0, 2, 4,  // Triangle 5
            4, 2, 6,  // Triangle 6
            1, 5, 3,  // Triangle 7
            5, 7, 3,  // Triangle 8
            0, 4, 1,  // Triangle 9
            4, 5, 1,  // Triangle 10
            2, 3, 6,  // Triangle 11
            6, 3, 7   // Triangle 12
        };

        return triList;
    }

    SphereRenderGeometry::SphereRenderGeometry()
		: RenderGeometry(RenderGeometryType::SPHERE)
    {}

	SphereRenderGeometry::SphereRenderGeometry(float radius)
		: RenderGeometry(RenderGeometryType::SPHERE), radius(radius)
	{}

	CapsuleRenderGeometry::CapsuleRenderGeometry()
		: RenderGeometry(RenderGeometryType::CAPSULE)
	{}

	CapsuleRenderGeometry::CapsuleRenderGeometry(float radius, float height)
		: RenderGeometry(RenderGeometryType::CAPSULE), radius(radius), height(height)
	{}

	PlaneRenderGeometry::PlaneRenderGeometry()
		: RenderGeometry(RenderGeometryType::PLANE)
	{}

    PosColorVertex* PlaneRenderGeometry::GetIdentityVertices()
    {
        static PosColorVertex vertices[VERT_SIZE] = {
            { -1.0f,  0.0f,  -1.0f, 0xff000000 },
            {  1.0f,  0.0f,  -1.0f, 0xff0000ff },
            { -1.0f, 0.0f,  1.0f, 0xff00ff00 },
            {  1.0f, 0.0f,  1.0f, 0xff00ffff }
        };

        return vertices;
    }

    uint16_t* PlaneRenderGeometry::GetIdentityTriList()
    {
        static uint16_t triList[TRI_SIZE] = {
            0, 1, 2,  // Triangle 1
            1, 3, 2  // Triangle 2
        };

        return triList;
    }

	ConeRenderGeometry::ConeRenderGeometry()
		: RenderGeometry(RenderGeometryType::CONE)
	{}

	ConeRenderGeometry::ConeRenderGeometry(float radius, float height)
		: RenderGeometry(RenderGeometryType::CONE), radius(radius), height(height)
	{}

	CylinderRenderGeometry::CylinderRenderGeometry()
		: RenderGeometry(RenderGeometryType::CYLINDER)
	{}

	CylinderRenderGeometry::CylinderRenderGeometry(float radius, float height)
		: RenderGeometry(RenderGeometryType::CYLINDER), radius(radius), height(height)
	{}

	DiskRenderGeometry::DiskRenderGeometry()
		: RenderGeometry(RenderGeometryType::DISK)
	{}

	DiskRenderGeometry::DiskRenderGeometry(float radius)
		: RenderGeometry(RenderGeometryType::DISK), radius(radius)
	{}

	TriangleRenderGeometry::TriangleRenderGeometry()
		: RenderGeometry(RenderGeometryType::TRIANGLE)
	{}

	TriangleRenderGeometry::TriangleRenderGeometry(float a, float b, float c)
		: RenderGeometry(RenderGeometryType::TRIANGLE), a(a), b(b), c(c)
	{}

	TriangleMeshRenderGeometry::TriangleMeshRenderGeometry()
		: RenderGeometry(RenderGeometryType::TRIANGLEMESH)
	{}
}


