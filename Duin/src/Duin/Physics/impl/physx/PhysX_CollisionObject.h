#pragma once

#include "Duin/Physics/CollisionObject.h"
#include "PhysX_PhysicsServer.h"

namespace duin {
    class PhysXCollisionObject : virtual public CollisionObject
    {
        public:
            PhysXPhysicsServer& GetPxServer();

        private:
    };
}
