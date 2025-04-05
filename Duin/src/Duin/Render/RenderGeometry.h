#pragma once

#include "Duin/Core/Utils/UUID.h"

#include <bgfx/bgfx.h>


namespace duin {
    namespace RenderGeometryType {
        typedef enum Type {
            BOX = 0,
            SPHERE,
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

    struct PosColorVertex {
        float x, y, z;
        uint32_t abgr;

        static void init()
        {
            ms_layout
                .begin()
                .add(bgfx::Attrib::Position, 3, bgfx::AttribType::Float)
                .add(bgfx::Attrib::Color0,   4, bgfx::AttribType::Uint8, true)
                .end();
        };

        static bgfx::VertexLayout ms_layout;
    };

    struct RenderGeometry {
        UUID uuid;
        RenderGeometryType::Type type;

        RenderGeometry() : type(RenderGeometryType::NONE) {}
        RenderGeometry(RenderGeometryType::Type t) 
            : type(t) {}
    };

    struct BoxRenderGeometry : public RenderGeometry {
        float width, height, depth = 1.0f;

        BoxRenderGeometry();
        BoxRenderGeometry(float width, float height, float depth);

        static const int VERT_SIZE = 8;
        static const int TRI_SIZE = 36;

        static PosColorVertex *GetIdentityVertices();
        static uint16_t *GetIdentityTriList();
        static size_t VertSize() { return VERT_SIZE; }
        static size_t TriSize() { return TRI_SIZE; }
    };

    struct SphereRenderGeometry : public RenderGeometry {
        float radius = 0.5f;;

        SphereRenderGeometry();
		SphereRenderGeometry(float radius);

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
