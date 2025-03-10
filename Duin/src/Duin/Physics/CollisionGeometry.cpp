#include "dnpch.h"
#include "CollisionGeometry.h"

namespace duin {
    BoxGeometry::BoxGeometry()
    {
        type = cBOX;
    }

    SphereGeometry::SphereGeometry()
    {
        type = cSPHERE;
    }

    CapsuleGeometry::CapsuleGeometry()
    {
        type = cCAPSULE;
    }

    PlaneGeometry::PlaneGeometry()
    {
        type = cPLANE;
    }

    ConvexMeshGeometry::ConvexMeshGeometry()
    {
        type = cCONVEXMESH;
    }

    TriangleMeshGeometry::TriangleMeshGeometry()
    {
        type = cTRIANGLEMESH;
    }
}
