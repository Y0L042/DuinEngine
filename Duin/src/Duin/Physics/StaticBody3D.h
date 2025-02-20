#pragma once

#include "./PhysicsBody3D.h"

namespace duin {
    class Physics3DServer;
    class StaticBody3D : public PhysicsBody3D
    {
        public:
            Physics3DServer& server;

            StaticBody3D(Physics3DServer& server);

        private:
    };
}
