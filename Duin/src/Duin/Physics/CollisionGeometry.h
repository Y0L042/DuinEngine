#pragma once

#include "Duin/Core/Maths/DuinMaths.h"

namespace duin {
    enum CollisionGeometryType {
        cNULL = 0,
        cBOX,
        cSPHERE,
        cCAPSULE,
        cPLANE,
        cCONVEXMESH,
        cTRIANGLEMESH,
        cCUSTOM
    };

    class CollisionGeometry
    {
        public:
            CollisionGeometryType type = cNULL;
    };

    class BoxGeometry : public CollisionGeometry
    {
        public:
            float width, depth, height;

            BoxGeometry();
            BoxGeometry(float width, float depth, float height)
                : width(width), depth(depth), height(height) {}
    };

    class SphereGeometry : public CollisionGeometry
    {
        public:
            float radius;

            SphereGeometry();
    };

    class CapsuleGeometry : public CollisionGeometry
    {
        public:
            float radius, height;

            CapsuleGeometry();
    };

    class PlaneGeometry : public CollisionGeometry
    {
        public:
            Vector3 normal;

            PlaneGeometry();
    };

    class ConvexMeshGeometry : public CollisionGeometry
    {
        public:
            ConvexMeshGeometry();
    };

    class TriangleMeshGeometry : public CollisionGeometry
    {
        public:
            TriangleMeshGeometry();
    };
}
