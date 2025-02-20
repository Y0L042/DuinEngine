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
// #include "./impl/jolt/JoltIncludes.h"
#endif
#endif

namespace duin {
    PhysicsServer* PhysicsServer::Create()
    {
        PHYSICS_SERVER* server = new PHYSICS_SERVER();
        server->Initialize();

        return server;
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
            case PhysicsServerErrors::SUCCESS:
                DN_CORE_INFO("[{}] Material creation failed!", INIT_FAILED);
                break;
            case PhysicsServerErrors::INIT_FAILED:
                DN_CORE_ERROR("[{}] PhysicsServerInitialization failed!", INIT_FAILED);
                break;
            case PhysicsServerErrors::MATERIAL_CREATION_FAILED:
                DN_CORE_ERROR("[{}] Material creation failed!", INIT_FAILED);
                break;
            default:
                break;
        }
    }
}
