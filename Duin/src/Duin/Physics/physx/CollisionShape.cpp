#include "dnpch.h"
#include "CollisionShape.h"

#include "PhysicsServer.h"

namespace duin
{
CollisionShape::CollisionShape(CollisionGeometryVariant geometryVariant, PhysicsMaterial physicsMaterial)
    : geometryVariant(geometryVariant), physicsMaterial(physicsMaterial)
{
    PhysicsServer &server = PhysicsServer::Get();
    switch (geometryVariant.index())
    {
    case cBOX: {
        BoxGeometry *g = std::get_if<cBOX>(&geometryVariant);
        if (g)
        {
            physx::PxBoxGeometry *pxGeometry = &g->pxBoxGeometry;
            physx::PxMaterial *pxMaterial = physicsMaterial.pxMaterial;
            pxShape = server.pxPhysics->createShape(*pxGeometry, *pxMaterial);
        }
        break;
    }
    case cSPHERE: {
        SphereGeometry *g = std::get_if<cSPHERE>(&geometryVariant);
        if (g)
        {
            physx::PxSphereGeometry *pxGeometry = &g->pxSphereGeometry;
            physx::PxMaterial *pxMaterial = physicsMaterial.pxMaterial;
            pxShape = server.pxPhysics->createShape(*pxGeometry, *pxMaterial);
        }
        break;
    }
    case cCAPSULE: {
        DN_CORE_FATAL("CAPSULE not implemented!");
        break;
    }
    case cPLANE: {
        PlaneGeometry *g = std::get_if<cPLANE>(&geometryVariant);
        if (g)
        {
            physx::PxPlaneGeometry *pxGeometry = &g->pxPlaneGeometry;
            physx::PxMaterial *pxMaterial = physicsMaterial.pxMaterial;
            pxShape = server.pxPhysics->createShape(*pxGeometry, *pxMaterial);
        }
        break;
    }
    case cCONVEXMESH: {
        break;
    }
    case cTRIANGLEMESH: {
        break;
    }
    case cCUSTOM: {
        break;
    }
    case cNULL: {
        break;
    }
    default: {
        break;
    }
    }
}

CollisionShape::~CollisionShape()
{
}

CollisionGeometryVariant &CollisionShape::GetGeometryVariant()
{
    return geometryVariant;
}
} // namespace duin
