#include "dnpch.h"
#include "CollisionShapes.h"

// Use PhysX by default
#ifndef JOLT
#define PHYSICS_CBOX PhysXBoxShape
#define PHYSICS_CSPHERE PhysXSphereShape
#define PHYSICS_CCAPSULE PhysXCapsuleShape
#define PHYSICS_CPLANE PhysXPlaneShape
#define PHYSICS_CCONVEXMESH PhysXConvexMeshShape
#define PHYSICS_CTRIANGLEMESH PhysXTriangleMeshShape
#include "./impl/physx/PhysX_Includes.h"
#endif

#ifdef JOLT
#ifndef PHYSX
#define PHYSICS_CBOX
#define PHYSICS_CSPHERE
#define PHYSICS_CCAPSULE
#define PHYSICS_CPLANE
#define PHYSICS_CCONVEXMESH
#define PHYSICS_CTRIANGLEMESH
#endif
#endif

namespace duin {
    BoxShape::BoxShape()
    {
        type = cBOX;
    }

    SphereShape::SphereShape()
    {
        type = cSPHERE;
    }

    CapsuleShape::CapsuleShape()
    {
        type = cCAPSULE;
    }

    PlaneShape::PlaneShape()
    {
        type = cPLANE;
    }

    ConvexMeshShape::ConvexMeshShape()
    {
        type = cCONVEXMESH;
    }

    TriangleMeshShape::TriangleMeshShape()
    {
        type = cTRIANGLEMESH;
    }
}
