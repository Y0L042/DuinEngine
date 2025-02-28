#pragma once

#include "Duin/Core/Maths/DuinMaths.h"

namespace duin {
    enum CollisionShapeType {
        cNULL = 0,
        cBOX,
        cSPHERE,
        cCAPSULE,
        cPLANE,
        cCONVEXMESH,
        cTRIANGLEMESH,
        cCUSTOM
    };

    class CollisionShape
    {
        public:
            CollisionShapeType type = cNULL;
    };

    class BoxShape : public CollisionShape
    {
        public:
            static BoxShape Create();

            float width, depth, height;

            BoxShape();
    };

    class SphereShape : public CollisionShape
    {
        public:
            static SphereShape Create();

            float radius;

            SphereShape();
    };

    class CapsuleShape : public CollisionShape
    {
        public:
            static CapsuleShape Create();

            float radius, height;

            CapsuleShape();
    };

    class PlaneShape : public CollisionShape
    {
        public:
            static PlaneShape Create();

            Vector3 normal;

            PlaneShape();
    };

    class ConvexMeshShape : public CollisionShape
    {
        public:
            static ConvexMeshShape Create();

            ConvexMeshShape();
    };

    class TriangleMeshShape : public CollisionShape
    {
        public:
            static TriangleMeshShape Create();

            TriangleMeshShape();
    };
}
