#pragma once

#include <variant>

#include <PxPhysicsAPI.h>

#include "PhysicsMaterial.h"
#include "CollisionGeometry.h"

namespace duin {
    using CollisionGeometryVariant = std::variant<
        BoxGeometry,
        SphereGeometry,
        CapsuleGeometry,
        PlaneGeometry,
        ConvexMeshGeometry,
        TriangleMeshGeometry
            >;

    class CollisionShape
    {
        public:
            CollisionShape(CollisionGeometryVariant geometryVariant, PhysicsMaterial physicsMaterial);
            ~CollisionShape();

            CollisionGeometryVariant& GetGeometryVariant();

        private:
            friend class StaticBody;
            friend class KinematicBody;
            friend class DynamicBody;

            CollisionGeometryVariant geometryVariant;
            PhysicsMaterial physicsMaterial;
            physx::PxShape *pxShape = nullptr;
    };
}
