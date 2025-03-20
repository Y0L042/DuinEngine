#pragma once

#include "Duin/Core/Maths/DuinMaths.h"

#include <PxPhysicsAPI.h>

namespace duin {


    enum CollisionGeometryType {
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

        private:
            friend class CollisionShape;

            physx::PxCapsuleGeometry pxCapsuleGeometry;
    };



    class PlaneGeometry : public CollisionGeometry
    {
        public:
            PlaneGeometry();

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
    };



    // TODO
    class TriangleMeshGeometry : public CollisionGeometry
    {
        public:
            TriangleMeshGeometry();
    };



}
