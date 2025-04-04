#include "dnpch.h"
#include "RenderGeometry.h"

namespace duin {
    SphereRenderGeometry::SphereRenderGeometry()
		: RenderGeometry(RenderGeometryType::SPHERE)
    {}

	SphereRenderGeometry::SphereRenderGeometry(float radius)
		: RenderGeometry(RenderGeometryType::SPHERE), radius(radius)
	{}

	BoxRenderGeometry::BoxRenderGeometry()
		: RenderGeometry(RenderGeometryType::BOX)
	{}

	BoxRenderGeometry::BoxRenderGeometry(float width, float height, float depth)
		: RenderGeometry(RenderGeometryType::BOX), width(width), height(height), depth(depth)
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


