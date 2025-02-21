#include "dnpch.h"
#include "PhysicsServer.h"

// Use PhysX by default
#ifndef JOLT
#define PHYSICS_SERVER PhysXServer
#include "./impl/physx/PhysXIncludes.h"
#endif

#ifdef JOLT
#ifndef PHYSX
#define PHYSICS_SERVER JoltServer
#endif
#endif

namespace duin {
    PhysicsServer& PhysicsServer::Get()
    {
        static PHYSICS_SERVER physicsServer;
        return physicsServer; 
    }

    int PhysicsServer::CatchError(void *ptr, int errorCode)
    {
        if (ptr == nullptr) {
            PrintErrorCode(errorCode);

            return 1;
        }

        return 0;
    }

    void PhysicsServer::PrintErrorCode(int errorCode)
    {
        switch (errorCode) {
            case PhysicsServerError::SUCCESS:
                // DN_CORE_INFO("[{}] Material creation failed!", INIT_FAILED);
                break;
            case PhysicsServerError::INIT_FAILED:
                // DN_CORE_ERROR("[{}] PhysicsServerInitialization failed!", INIT_FAILED);
                break;
            case PhysicsServerError::MATERIAL_CREATION_FAILED:
                // DN_CORE_ERROR("[{}] Material creation failed!", INIT_FAILED);
                break;
            default:
                break;
        }
    }
}
