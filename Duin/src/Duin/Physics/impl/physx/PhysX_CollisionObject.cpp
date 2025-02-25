#include "dnpch.h"

#include "PhysX_CollisionObject.h"

namespace duin {
    PhysXPhysicsServer& PhysXCollisionObject::GetPxServer()
    {
        return PhysXPhysicsServer::GetPxServer();
    }
}
