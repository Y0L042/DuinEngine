#pragma once

namespace duin {
    namespace RenderGeometryType {
        typedef enum Type {
            SPHERE,
            BOX,
            CAPSULE,
            PLANE,
            CONE,
            CYLINDER,
            DISK,
            TRIANGLE,
            TRIANGLEMESH,
            NONE,
            Count
        } Type;
    };

    struct RenderGeometry {
        RenderGeometryType::Type type;

        RenderGeometry() : type(RenderGeometryType::NONE) {}
        RenderGeometry(RenderGeometryType::Type t) 
            : type(t) {}
    };

    struct SphereRenderGeometry : public RenderGeometry {
        float radius = 0.5f;;

        SphereRenderGeometry();
		SphereRenderGeometry(float radius);
    };

    struct BoxRenderGeometry : public RenderGeometry {
        float width, height, depth = 1.0f;

        BoxRenderGeometry();
        BoxRenderGeometry(float width, float height, float depth);
    };

    struct CapsuleRenderGeometry : public RenderGeometry {
        float radius = 0.5f;
        float height = 1.0f;

        CapsuleRenderGeometry();
		CapsuleRenderGeometry(float radius, float height);
    };

    struct PlaneRenderGeometry : public RenderGeometry {
        PlaneRenderGeometry();
    };

    struct ConeRenderGeometry : public RenderGeometry {
        float radius = 0.5f;
        float height = 1.0f;

		ConeRenderGeometry();
		ConeRenderGeometry(float radius, float height);
    };

    struct CylinderRenderGeometry : public RenderGeometry {
        float radius = 0.5f;
        float height = 1.0f;

		CylinderRenderGeometry();
		CylinderRenderGeometry(float radius, float height);
    };

    struct DiskRenderGeometry : public RenderGeometry {
        float radius = 0.5f;

		DiskRenderGeometry();
		DiskRenderGeometry(float radius);
    };

    struct TriangleRenderGeometry : public RenderGeometry {
        float a, b, c = 1.0f;

		TriangleRenderGeometry();
		TriangleRenderGeometry(float a, float b, float c);
    };

    struct TriangleMeshRenderGeometry : public RenderGeometry {
        //std::array<float> vertices; // TODO

        TriangleMeshRenderGeometry();
    };
}
