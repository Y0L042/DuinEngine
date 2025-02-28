#pragma once

#include "Duin/Physics/CollisionShapes.h"

namespace duin {
    class PhysXCollisionShape : public CollisionShape
    {};

    class PhysXBoxShape 
        : public PhysXCollisionShape, virtual public BoxShape
    {};

    class PhysXSphereShape 
        : public PhysXCollisionShape, virtual public SphereShape
    {};

    class PhysXCapsuleShape 
        : public PhysXCollisionShape, virtual public CapsuleShape
    {};

    class PhysXPlaneShape 
        : public PhysXCollisionShape, virtual public PlaneShape
    {};

    class PhysXConvexMeshShape 
        : public PhysXCollisionShape, virtual public ConvexMeshShape
    {};

    class PhysXTriangleMeshShape 
        : public PhysXCollisionShape, virtual public TriangleMeshShape
    {};
}
