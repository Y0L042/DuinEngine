#pragma once

#include "Duin/Physics/CollisionGeometry.h"

namespace duin {
    class PhysXCollisionGeometry
        : public CollisionGeometry
    {
        public:
        private:
    };

    class PhysXBoxGeometry 
        : public PhysXCollisionGeometry, public virtual BoxGeometry
    {
        public:
            float width, depth, height;

            PhysXBoxGeometry();
            PhysXBoxGeometry(float width, float depth, float height)
                : width(width), depth(depth), height(height) {}
    };

    class PhysXSphereGeometry 
        : public PhysXCollisionGeometry, public virtual SphereGeometry
    {
        public:
            float radius;

            PhysXSphereGeometry();
    };

    class PhysXCapsuleGeometry 
        : public PhysXCollisionGeometry, public virtual CapsuleGeometry
    {
        public:
            float radius, height;

            PhysXCapsuleGeometry();
    };

    class PhysXPlaneGeometry 
        : public PhysXCollisionGeometry, public virtual PlaneGeometry
    {
        public:
            Vector3 normal;

            PhysXPlaneGeometry();
    };

    class PhysXConvexMeshGeometry 
        : public PhysXCollisionGeometry, public virtual ConvexMeshGeometry
    {
        public:
            PhysXConvexMeshGeometry();
    };

    class PhysXTriangleMeshGeometry 
        : public PhysXCollisionGeometry, public virtual TriangleMeshGeometry
    {
        public:
            PhysXTriangleMeshGeometry();
    };
}
