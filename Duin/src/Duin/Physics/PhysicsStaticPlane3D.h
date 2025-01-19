#pragma once

#include <PxPhysicsAPI.h>
#include <PxPhysics.h>
#include <characterkinematic/PxController.h>
#include <characterkinematic/PxControllerManager.h>
#include <cooking/PxCooking.h>

#include "Duin/Core/Maths/DuinMaths.h"


#include <functional>

namespace duin {  
    struct StaticCollisionPlaneDesc {

    };

    class Physics3DServer;
    class StaticCollisionPlane
    {
        public:
            Physics3DServer& server;

            StaticCollisionPlane(Physics3DServer& server);

        private:

    }; 
}
