#pragma once

#include <PxPhysicsAPI.h>
#include <PxPhysics.h>
#include <characterkinematic/PxController.h>
#include <characterkinematic/PxControllerManager.h>
#include <cooking/PxCooking.h>

#include "Duin/Core/Maths/DuinMaths.h"

namespace duin {
    class PhysicsStaticBody3D
    {
    public:


    private:
    };

    class Physics3DServer;
    class PhysicsStaticPlane3D : public PhysicsStaticBody3D
    {
        public:
            Physics3DServer& server;

            PhysicsStaticPlane3D(Physics3DServer& server);

        private:

    }; 

    class Physics3DServer;
    class PhysicsStaticCube3D : public PhysicsStaticBody3D
    {
        public:
            Physics3DServer& server;

            PhysicsStaticCube3D(Physics3DServer& server, Vector3 position, Vector3 size);

        private:

    }; 
}

