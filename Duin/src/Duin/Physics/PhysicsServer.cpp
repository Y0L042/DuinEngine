#include "dnpch.h"
#include "PhysicsServer.h"

#include "Duin/Core/Debug/DNLog.h"
#include "Duin/Core/Utils/UUID.h"
#include "CollisionObject.h"

// Use PhysX by default
#ifndef JOLT
#define PHYSICS_SERVER PhysXPhysicsServer
#include "./impl/physx/PhysX_Includes.h"
#endif

#ifdef JOLT
#ifndef PHYSX
#define PHYSICS_SERVER JoltServer
#endif
#endif

namespace duin {
    /** Singleton variable */
    std::unique_ptr<PHYSICS_SERVER> PhysicsServer::implServer;

    PhysicsServer& PhysicsServer::Get()
    {
        if (implServer == nullptr) {
            implServer = std::make_unique<PHYSICS_SERVER>();
        }
        return *implServer.get(); 
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
